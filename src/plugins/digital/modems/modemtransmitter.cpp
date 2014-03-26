#include "modemtransmitter.h"

using namespace Digital::Internal;

ModemTransmitter::ModemTransmitter(QObject* parent)
    : AudioProducer(parent, 5)
{
}

void ModemTransmitter::writeSample(double sample)
{
}

