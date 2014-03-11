#ifndef MODEMRECEIVER_H
#define MODEMRECEIVER_H

#include "../audio/audioconsumer.h"

namespace Digital {
namespace Internal {

class ModemReceiver
        : public AudioConsumer
{
    Q_OBJECT

public:
    ModemReceiver(QObject*);
};

} // namespace Internal
} // namespace Digital

#endif // MODEMRECEIVER_H
