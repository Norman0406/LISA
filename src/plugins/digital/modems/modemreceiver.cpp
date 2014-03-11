#include "modemreceiver.h"

using namespace Digital::Internal;

ModemReceiver::ModemReceiver(QObject* parent)
    : AudioConsumer(512, parent)
{
}
