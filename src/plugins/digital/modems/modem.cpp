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

#include "modem.h"
#include "modemfactory.h"
#include "modemconfig.h"
#include "../signalprocessing/misc.h"
#include <QDebug>

using namespace Digital::Internal;

Modem::Modem(unsigned capability, QObject* parent)
    : QObject(parent),
      m_capability(capability),
      m_frequency(1000),
      m_afcSpeed(AFC_NORMAL),
      m_freqErr(0),
      m_afc(true),
      m_reverse(false),
      m_state(STATE_PREINIT)
{
}

Modem::~Modem()
{
}

bool Modem::init(const QAudioFormat& format)
{
    if (!format.isValid())
        return false;

    m_format = format;

    if (iInit()) {
        restart();
        return true;
    }

    return false;
}

void Modem::restart()
{
    m_freqErr = 0.0;

    iRestart();

    m_state = STATE_READY;
}

void Modem::shutdown()
{
    iShutdown();

    m_state = STATE_PREINIT;
}

void Modem::rxProcess(const QVector<double>& buffer)
{
    if (m_format.isValid())
        iRxProcess(buffer);
}

void Modem::txProcess(ModemTransmitter* transmitter)
{
    if (m_format.isValid() && transmitter)
        iTxProcess(transmitter);
}

/*void Modem::send(QString message, ModemTransmitter* transmitter)
{
    for (int i = 0; i < message.count(); i++)
        send(message[i].toAscii(), transmitter);
}*/

void Modem::setFrequency(double frequency)
{
    m_frequency = frequency;
    emit frequencyChanged(m_frequency);
}

void Modem::setAFCSpeed(AFCSpeed afcSpeed)
{
    m_afcSpeed = afcSpeed;
}

bool Modem::hasCapability(Capability cap) const
{
    return m_capability & cap;
}

double Modem::getFrequency() const
{
    return m_frequency;
}

Modem::AFCSpeed Modem::getAFCSpeed() const
{
    return m_afcSpeed;
}

int Modem::getSampleRate() const
{
    return m_format.sampleRate();
}

void Modem::adjustFrequency(double frqerr)
{
    m_freqErr = decayAvg(m_freqErr, frqerr / 8, m_afcSpeed == AFC_SLOW ? 8 : m_afcSpeed == AFC_NORMAL ? 4 : 1);

    if (m_afc) {
        setFrequency(m_frequency - m_freqErr);
        //qDebug() << m_frequency;
    }
}

void Modem::process()
{
    while (m_state != STATE_SHUTDOWN) {
        m_lock.lock();
        while (m_state != STATE_RX && m_state != STATE_TX)
            m_waitForData.wait(&m_lock);

        if (m_state == STATE_RX) {
            rxProcess(m_inputBlock);
            m_inputBlock.clear();
            m_state = STATE_READY;
        }
        else if (m_state == STATE_TX) {
            for (int i = 0; i < m_message.count(); i++) {
                send(m_message[i].toLatin1(), m_transmitter);
                qDebug() << m_message[i];
                m_transmitter->flush();
            }
            m_message.clear();
            m_transmitter = 0;
            m_state = STATE_READY;
        }

        m_lock.unlock();
    }
}

void Modem::receive(const QVector<double>& data)
{
    /*m_lock.lock();
    if (m_state == STATE_READY) {
        m_state = STATE_RX;
        m_inputBlock = data;
        m_waitForData.wakeAll();
    }
    m_lock.unlock();*/
}

void Modem::startTx()
{
    // TODO: maybe introduce a STATE_IDLE that is set when tx is started but
    // no data is yet available to send. In this case, the modem would only send
    // idle signals
}

void Modem::transmit(QString message, ModemTransmitter* transmitter)
{
    m_lock.lock();
    m_state = STATE_TX;

    m_message = message;
    m_transmitter = transmitter;

    m_waitForData.wakeAll();
    m_lock.unlock();
}

void Modem::stopTx()
{
    // TODO: stop tx, i.e. stop any idle or character signals and go back to STATE_RX
}

double Modem::getFrqErr() const
{
    return m_freqErr;
}

bool Modem::setAFC(bool enabled)
{
    if (hasCapability(CAP_AFC)) {
        m_afc = enabled;
        return true;
    }
    return false;
}

bool Modem::setReverse(bool enabled)
{
    if (hasCapability(CAP_REV)) {
        m_reverse = enabled;
        return true;
    }
    return false;
}

bool Modem::getAFC() const
{
    return m_afc;
}

bool Modem::getReverse() const
{
    return m_reverse;
}

Modem::State Modem::getState() const
{
    return m_state;
}

double Modem::squelchOpen() const
{
    return true;
}
