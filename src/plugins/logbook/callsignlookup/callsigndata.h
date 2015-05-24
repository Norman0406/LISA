#ifndef CALLSIGNDATA_H
#define CALLSIGNDATA_H

#include <QMap>
#include <QString>

namespace Logbook {
namespace Internal {

class CallsignData
{
public:
    CallsignData(const QMap<QString, QString>& fields);

    bool getField(const QString& fieldName, QString& result) const;

private:
    QMap<QString, QString> m_fields;
};

}
}

#endif // CALLSIGNDATA_H
