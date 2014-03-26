#include "modemreceiver.h"
#include "modem.h"

using namespace Digital::Internal;

ModemReceiver::ModemReceiver(QObject* parent, Modem* modem)
    : AudioConsumer(512, parent),
      m_modem(modem)
{
}

void ModemReceiver::audioDataReady(const QVector<double>& data)
{
    m_modem->rxProcess(data);
}
