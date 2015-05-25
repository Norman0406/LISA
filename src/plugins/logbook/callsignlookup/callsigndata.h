#ifndef CALLSIGNDATA_H
#define CALLSIGNDATA_H

#include <QMap>
#include <QString>

namespace Logbook {
namespace Internal {

class CallsignData
{
public:
    enum CallsignService {
        CS_QRZCOM,
    };

    CallsignData(CallsignService, const QMap<QString, QString>& fields);

    bool getField(const QString& fieldName, QString& result) const;
    QString getField(const QString& fieldName) const;
    CallsignService getService() const;
    static QString getServiceString(CallsignService);

private:
    QMap<QString, QString> m_fields;
    const CallsignService m_service;
};

}
}

#endif // CALLSIGNDATA_H
