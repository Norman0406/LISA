#include "callsignlookup.h"

using namespace Logbook::Internal;

CallsignLookup::CallsignLookup(QObject* parent, CallsignData::CallsignService service)
    : QObject(parent),
      m_service(service)
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
