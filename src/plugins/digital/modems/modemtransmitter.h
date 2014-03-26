#ifndef MODEMTRANSMITTER_H
#define MODEMTRANSMITTER_H

#include "../audio/audioproducer.h"

namespace Digital {
namespace Internal {

class ModemTransmitter
        : public AudioProducer
{
    Q_OBJECT

public:
    ModemTransmitter(QObject*);

    void writeSample(double);
};

} // Internal
} // Digital

#endif // MODEMTRANSMITTER_H
