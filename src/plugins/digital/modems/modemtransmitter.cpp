#include "modemtransmitter.h"

using namespace Digital::Internal;

ModemTransmitter::ModemTransmitter(QObject* parent)
    : AudioProducer(parent, 0.5)
{
}
