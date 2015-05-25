#include "callsignlookup.h"

using namespace Logbook::Internal;

CallsignLookup::CallsignLookup(QObject* parent, CallsignData::CallsignService service)
    : QObject(parent),
      m_service(service),
      m_enabled(true)
{
}

CallsignLookup::~CallsignLookup()
{
}

bool CallsignLookup::lookup(QString callsign)
{
    if (m_enabled)
        return iLookup(callsign);

    return true;
}

void CallsignLookup::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool CallsignLookup::isEnabled() const
{
    return m_enabled;
}

CallsignData::CallsignService CallsignLookup::getService() const
{
    return m_service;
}

