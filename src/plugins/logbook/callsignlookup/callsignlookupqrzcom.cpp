#include "callsignlookupqrzcom.h"
#include "coreplugin/coreconstants.h"

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
    : m_agent(QString::fromLatin1("LISA")),
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
                            emit sessionKeyChanged();
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
                        CallsignData callsign(fields);
                        emit callsignRetrieved(callsign);
                    }
                }
            }
        }
    }
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

void CallsignLookupQRZcom::refreshSessionKey()
{
    QUrlQuery query;
    query.addQueryItem(QString::fromLatin1("username"), m_username);
    query.addQueryItem(QString::fromLatin1("password"), m_password);
    query.addQueryItem(QString::fromLatin1("agent"), m_agent);

    QNetworkRequest request(getLoginUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/x-www-form-urlencoded"));

    QByteArray params = query.toString(QUrl::FullyEncoded).toLatin1();
    m_manager->post(request, params);
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

bool CallsignLookupQRZcom::iLookup(QString callsign)
{
    if (!isReady())
        return false;

    if (m_sessionKey.isEmpty()) {
        connect(this, &CallsignLookupQRZcom::sessionKeyChanged, [=]() {
            queryCallsign(callsign);
        });

        refreshSessionKey();
    }
    else
        queryCallsign(callsign);

    return true;
}

