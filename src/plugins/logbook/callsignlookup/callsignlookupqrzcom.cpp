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

CallsignLookupQRZcom::CallsignLookupQRZcom(QObject* parent)
    : CallsignLookup(parent, CallsignData::CS_QRZCOM),
      m_agent(QLatin1String("LISA")),
      m_version(QLatin1String("1.33"))
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
    if (username != m_username)
        m_sessionKey.clear();

    m_username = username;
    return true;
}

bool CallsignLookupQRZcom::setPassword(QString password)
{
    if (password != m_password)
        m_sessionKey.clear();

    m_password = password;
    return true;
}

QString CallsignLookupQRZcom::getUsername() const
{
    return m_username;
}

QString CallsignLookupQRZcom::getPassword() const
{
    return m_password;
}

void CallsignLookupQRZcom::replyFinished(QNetworkReply* reply)
{
    QString dataString = QString::fromUtf8(reply->readAll());

    // TODO: check for error messages and remarks and throw signals

    QDomDocument xmlDoc;
    if (!xmlDoc.setContent(dataString)) {
        qWarning() << "could not parse xml reply";
    }
    else {
        QDomElement xmlDocElem = xmlDoc.documentElement();
        if (xmlDocElem.isNull() || xmlDocElem.nodeName() != QLatin1String("QRZDatabase")) {
            qWarning() << "QRZDatabase element not found";
        }
        else {
            QDomElement sessionElem = xmlDocElem.firstChildElement(QLatin1String("Session"));

            // the session element must be valid in each reply
            if (sessionElem.isNull()) {
                qWarning() << "Session element not found";
            }
            else {
                QDomElement error = sessionElem.firstChildElement(QLatin1String("Error"));
                if (!error.isNull()) {
                    QString errorString = error.text();
                    qDebug() << errorString;
                }
                else {
                    QDomElement sessionKey = sessionElem.firstChildElement(QLatin1String("Key"));
                    QDomElement count = sessionElem.firstChildElement(QLatin1String("Count"));
                    QDomElement subExp = sessionElem.firstChildElement(QLatin1String("SubExp"));
                    QDomElement gmtime = sessionElem.firstChildElement(QLatin1String("GMTime"));
                    QDomElement message = sessionElem.firstChildElement(QLatin1String("Message"));
                    QDomElement remark = sessionElem.firstChildElement(QLatin1String("Remark"));

                    if (sessionKey.isNull())
                        qWarning() << "invalid session key";
                    else {
                        QString sessionKeyString = sessionKey.text();
                        if (m_sessionKey != sessionKeyString) {
                            m_sessionKey = sessionKeyString;
                        }
                    }
                }

                QDomElement callsignElem = xmlDocElem.firstChildElement(QLatin1String("Callsign"));
                if (!callsignElem.isNull()) {
                    QMap<QString, QString> fields;

                    QDomNodeList children = callsignElem.childNodes();
                    for (int i = 0; i < children.count(); i++) {
                        QDomNode child = children.at(i);
                        if (child.isElement()) {
                            QDomElement childElem = child.toElement();
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
    return QUrl(QString(QLatin1String("https://xmldata.qrz.com/xml/%1/")).arg(m_version));
}

QUrl CallsignLookupQRZcom::getQueryUrl() const
{
    // use standard http for callsign queries
    return QUrl(QString(QLatin1String("http://xmldata.qrz.com/xml/%1/")).arg(m_version));
}

QNetworkReply* CallsignLookupQRZcom::refreshSessionKey()
{
    QUrlQuery query;
    query.addQueryItem(QLatin1String("username"), m_username);
    query.addQueryItem(QLatin1String("password"), m_password);
    query.addQueryItem(QLatin1String("agent"), m_agent);

    QNetworkRequest request(getLoginUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    QByteArray params = query.toString(QUrl::FullyEncoded).toLatin1();
    return m_manager->post(request, params);
}

void CallsignLookupQRZcom::queryCallsign(QString callsign)
{
    QUrlQuery query;
    query.addQueryItem(QLatin1String("s"), m_sessionKey);
    query.addQueryItem(QLatin1String("callsign"), callsign);

    QNetworkRequest request(getQueryUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

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

