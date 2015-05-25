#ifndef CALLSIGNLOOKUPQRZCOM_H
#define CALLSIGNLOOKUPQRZCOM_H

#include "callsignlookup.h"
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QQueue>

namespace Logbook {
namespace Internal {

class CallsignLookupQRZcom
        : public CallsignLookup
{
    Q_OBJECT

public:
    CallsignLookupQRZcom(QObject* parent = 0);
    ~CallsignLookupQRZcom();

    bool setUsername(QString);
    bool setPassword(QString);

    QString getUsername() const;
    QString getPassword() const;

signals:
    void replyProcessed(QNetworkReply*);

protected:
    bool iLookup(QString);

private slots:
    void replyFinished(QNetworkReply*);
    void processLookup();

private:
    bool isReady() const;
    QUrl getLoginUrl() const;
    QUrl getQueryUrl() const;
    QNetworkReply* refreshSessionKey();
    void queryCallsign(QString);

    QNetworkAccessManager* m_manager;
    QString m_username;
    QString m_password;
    QString m_sessionKey;
    const QString m_agent;
    const QString m_version;
    bool m_refreshRunning;
    QQueue<QString> m_queue;
};

}
}


#endif // CALLSIGNLOOKUPQRZCOM_H
