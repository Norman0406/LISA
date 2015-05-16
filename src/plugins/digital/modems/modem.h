/***********************************************************************
 *
 * LISA: Lightweight Integrated System for Amateur Radio
 * Copyright (C) 2013 - 2014
 *      Norman Link (DM6LN)
 *
 * This file is part of LISA.
 *
 * LISA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LISA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You can find a copy of the GNU General Public License in the file
 * LICENSE.GPL contained in the root directory of this project or
 * under <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#ifndef MODEM_H
#define MODEM_H

#include <QObject>
#include <QString>
#include <QVector>
#include <cmath>

#include <QAudioFormat>
#include <QWaitCondition>

#include "modemtransmitter.h"

namespace Digital {
namespace Internal {

#define TWO_PI 2.0 * M_PI

class Modem
        : public QObject
{
    Q_OBJECT

public:
    enum Capability
    {
        CAP_RX = 1 << 0,        // can receive
        CAP_TX = 1 << 1,        // can transmit
        CAP_AFC = 1 << 2,       // has automatic frequency control
        CAP_REV = 1 << 3,       // has reverse mode (USB - LSB)
        CAP_MULT = 1 << 4,      // ability to scan multiple channels in parallel (channel browser)
        CAP_SCOPE = 1 << 5      // defines a scope widget
    };

    enum State
    {
        STATE_PREINIT,
        STATE_READY,
        STATE_RX,
        STATE_TX,
        STATE_SHUTDOWN
    };

    enum AFCSpeed
    {
        AFC_SLOW,
        AFC_NORMAL,
        AFC_FAST
    };

    virtual ~Modem();

    bool init(const QAudioFormat&);
    void restart();
    void rxProcess(const QVector<double>&);
    void txProcess(ModemTransmitter*);
    void shutdown();

    virtual QString getType() const = 0;

    void            setFrequency(double);
    void            setAFCSpeed(AFCSpeed);
    bool            setAFC(bool);
    bool            setReverse(bool);

    bool            hasCapability(Capability) const;
    double          getFrequency() const;
    virtual double  getBandwidth() const = 0;
    AFCSpeed        getAFCSpeed() const;
    bool            getAFC() const;
    bool            getReverse() const;
    State           getState() const;

    void receive(const QVector<double>&);
    void startTx();
    void transmit(QString, ModemTransmitter*);
    void stopTx();

public slots:
    void process();
    //virtual void send(QString, ModemTransmitter*);
    virtual void send(char, ModemTransmitter*) = 0;

signals:
    void received(char);
    void sent(char);
    void frequencyChanged(double);
    void bandwidthChanged(double);

protected:
    Modem(unsigned, QObject*);

    virtual bool iInit() = 0;
    virtual void iRestart() = 0;
    virtual void iShutdown() = 0;
    virtual void iRxProcess(const QVector<double>&) = 0;
    virtual void iTxProcess(ModemTransmitter*) = 0;
    void writeSample(double);

    double getFrqErr() const;
    void adjustFrequency(double);   // AFC
    int getSampleRate() const;
    double squelchOpen() const; // metric > squelch ? true : false

private:
    QMutex      m_lock;
    QWaitCondition m_waitForData;
    QVector<double> m_inputBlock;

    ModemTransmitter* m_transmitter;
    QString m_message;

    QAudioFormat    m_format;
    unsigned    m_capability;
    double      m_frequency;
    AFCSpeed    m_afcSpeed;
    double      m_freqErr;
    bool        m_afc;
    bool        m_reverse;
    State       m_state;
};

} // namespace Internal
} // namespace Digital

#endif // MODEM_H
