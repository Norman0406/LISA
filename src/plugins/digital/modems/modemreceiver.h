#ifndef MODEMRECEIVER_H
#define MODEMRECEIVER_H

#include "../audio/audioconsumer.h"

namespace Digital {
namespace Internal {

class Modem;

class ModemReceiver
        : public AudioConsumer
{
    Q_OBJECT

public:
    ModemReceiver(QObject*, Modem*);

protected:
    void audioDataReady(const QVector<double>& data);

private:
    Modem* m_modem;
};

} // namespace Internal
} // namespace Digital

#endif // MODEMRECEIVER_H
