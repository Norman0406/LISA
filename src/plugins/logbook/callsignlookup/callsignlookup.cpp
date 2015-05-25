#include "callsignlookup.h"

using namespace Logbook::Internal;

CallsignLookup::CallsignLookup(CallsignData::CallsignService service)
    : m_service(service)
{
}

CallsignLookup::~CallsignLookup()
{
}

bool CallsignLookup::lookup(QString callsign)
{
    return iLookup(callsign);
}

CallsignData::CallsignService CallsignLookup::getService() const
{
    return m_service;
}
