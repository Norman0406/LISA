#include "callsignlookupqrzcom.h"
#include "coreplugin/coreconstants.h"

#include <memory>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpMultiPart>
#include <QUrlQuery>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>
#include <QXmlStreamReader>
#include <QSignalMapper>

using namespace Logbook::Internal;

CallsignLookupQRZcom::CallsignLookupQRZcom()
    : CallsignLookup(CallsignData::CS_QRZCOM),
      m_agent(QString::fromLatin1("LISA")),
      m_version(QString::fromLatin1("1.33"))
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &CallsignLookupQRZcom::replyFinished);
}

CallsignLookupQRZcom::~CallsignLookupQRZcom()
{
    delete m_manager;
}

bool CallsignLookupQRZcom::setUsername(QString username)
{
    m_username = username;
    return true;
}

bool CallsignLookupQRZcom::setPassword(QString password)
{
    m_password = password;
    return true;
}

void CallsignLookupQRZcom::replyFinished(QNetworkReply* reply)
{
    QString dataString = QString::fromUtf8(reply->readAll());

    QDomDocument xmlDoc;
    if (!xmlDoc.setContent(dataString)) {
        qWarning() << "could not parse xml reply";
    }
    else {
        QDomElement xmlDocElem = xmlDoc.documentElement();
        if (xmlDocElem.isNull() || xmlDocElem.nodeName() != QString::fromLatin1("QRZDatabase")) {
            qWarning() << "QRZDatabase element not found";
        }
        else {
            QDomElement sessionElem = xmlDocElem.firstChildElement(QString::fromLatin1("Session"));

            // the session element must be valid in each reply
            if (sessionElem.isNull()) {
                qWarning() << "Session element not found";
            }
            else {
                QDomElement error = sessionElem.firstChildElement(QString::fromLatin1("Error"));
                if (!error.isNull()) {
                    QString errorString = error.text();
                    qDebug() << errorString;
                }
                else {
                    QDomElement sessionKey = sessionElem.firstChildElement(QString::fromLatin1("Key"));
                    QDomElement count = sessionElem.firstChildElement(QString::fromLatin1("Count"));
                    QDomElement subExp = sessionElem.firstChildElement(QString::fromLatin1("SubExp"));
                    QDomElement gmtime = sessionElem.firstChildElement(QString::fromLatin1("GMTime"));
                    QDomElement message = sessionElem.firstChildElement(QString::fromLatin1("Message"));
                    QDomElement remark = sessionElem.firstChildElement(QString::fromLatin1("Remark"));

                    if (sessionKey.isNull())
                        qWarning() << "invalid session key";
                    else {
                        QString sessionKeyString = sessionKey.text();
                        if (m_sessionKey != sessionKeyString) {
                            m_sessionKey = sessionKeyString;
                            /*m_refreshRunning = false;
                            emit sessionKeyChanged();*/
                            qDebug() << "sessionKey: " << sessionKeyString;
                        }
                    }
                }

                QDomElement callsignElem = xmlDocElem.firstChildElement(QString::fromLatin1("Callsign"));
                if (!callsignElem.isNull()) {
                    QMap<QString, QString> fields;

                    QDomNodeList children = callsignElem.childNodes();
                    for (int i = 0; i < children.count(); i++) {
                        QDomNode child = children.at(i);
                        if (child.isElement()) {
                            QDomElement childElem = child.toElement();
                            qDebug() << childElem.nodeName() << ", " << childElem.text();
                            fields[childElem.nodeName()] = childElem.text();
                        }
                    }

                    if (fields.size() > 0) {
                        CallsignData callsign(getService(), fields);
                        emit callsignRetrieved(callsign);
                    }
                }
            }
        }
    }

    emit replyProcessed(reply);
}

bool CallsignLookupQRZcom::isReady() const
{
    return !m_password.isEmpty() && !m_username.isEmpty();
}

QUrl CallsignLookupQRZcom::getLoginUrl() const
{
    // use https for login routine
    return QUrl(QString::fromLatin1("https://xmldata.qrz.com/xml/%1/").arg(m_version));
}

QUrl CallsignLookupQRZcom::getQueryUrl() const
{
    // use standard http for callsign queries
    return QUrl(QString::fromLatin1("http://xmldata.qrz.com/xml/%1/").arg(m_version));
}

QNetworkReply* CallsignLookupQRZcom::refreshSessionKey()
{
    QUrlQuery query;
    query.addQueryItem(QString::fromLatin1("username"), m_username);
    query.addQueryItem(QString::fromLatin1("password"), m_password);
    query.addQueryItem(QString::fromLatin1("agent"), m_agent);

    QNetworkRequest request(getLoginUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/x-www-form-urlencoded"));

    QByteArray params = query.toString(QUrl::FullyEncoded).toLatin1();
    return m_manager->post(request, params);
}

void CallsignLookupQRZcom::queryCallsign(QString callsign)
{
    QUrlQuery query;
    query.addQueryItem(QString::fromLatin1("s"), m_sessionKey);
    query.addQueryItem(QString::fromLatin1("callsign"), callsign);

    QNetworkRequest request(getQueryUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/x-www-form-urlencoded"));

    QByteArray params = query.toString(QUrl::FullyEncoded).toLatin1();
    m_manager->post(request, params);
}

void CallsignLookupQRZcom::processLookup()
{
    if (m_sessionKey.isEmpty() && !m_refreshRunning) {
        m_refreshRunning = true;

        QNetworkReply* refreshReply = refreshSessionKey();

        auto connection = std::make_shared<QMetaObject::Connection>();
        *connection = connect(this, &CallsignLookupQRZcom::replyProcessed, [=](QNetworkReply* reply) {
            if (reply == refreshReply) {
                m_refreshRunning = false;

                if (!m_sessionKey.isEmpty())
                    processLookup();
                else
                    qWarning() << "could not retrieve a valid session key";

                disconnect(*connection);
            }
        });
    }
    else if (!m_refreshRunning) {
        while (!m_queue.empty()) {
            QString callsign = m_queue.front();
            m_queue.pop_front();
            queryCallsign(callsign);
        }
    }
}

bool CallsignLookupQRZcom::iLookup(QString callsign)
{
    if (!isReady())
        return false;

    m_queue.push_back(callsign);
    processLookup();

    return true;
}

