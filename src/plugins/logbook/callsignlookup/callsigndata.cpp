#include "callsigndata.h"

using namespace Logbook::Internal;

CallsignData::CallsignData(CallsignService service, const QMap<QString, QString>& fields)
    : m_service(service)
{
    m_fields = fields;
}

bool CallsignData::getField(const QString& fieldName, QString& result) const
{
    if (m_fields.contains(fieldName)) {
        result = m_fields[fieldName];
        return true;
    }

    return false;
}

CallsignData::CallsignService CallsignData::getService() const
{
    return m_service;
}

