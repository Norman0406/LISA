#include "callsignlookup.h"

using namespace Logbook::Internal;

CallsignLookup::CallsignLookup()
{
}

CallsignLookup::~CallsignLookup()
{
}

bool CallsignLookup::lookup(QString callsign)
{
    return iLookup(callsign);
}

