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

#include "audioproducer.h"
#include "audioproducerlist.h"

using namespace Digital::Internal;

AudioProducer::AudioProducer(QObject* parent, double frq, double interval)
    : QObject(parent),
      m_bufferLengthSec(1),
      m_frq(frq),
      m_producerList(0)
{
    m_timerThread = new QThread(this);
    m_timer = new QTimer(0);
    m_timer->moveToThread(m_timerThread);
    m_timer->setInterval(1000 * interval);
    connect(m_timerThread, SIGNAL(started()), m_timer, SLOT(start()));
    connect(m_timer, &QTimer::timeout, this, &AudioProducer::fill, Qt::DirectConnection);
}

AudioProducer::~AudioProducer()
{
}

#include <QtMath>
#include <QDebug>

void AudioProducer::create(QAudioFormat format, AudioProducerList* producerList)
{
    m_format = format;
    m_producerList = producerList;
    m_buffer = new AudioRingBuffer(m_format, m_bufferLengthSec, this);
    m_timerThread->start();
}

void AudioProducer::fill()
{
    int numSamples = m_bufferLengthSec * getFormat().sampleRate();

    QVector<double> data(numSamples);
    int sampleIndex = 0;
    double frq = m_frq;
    double endFrq = frq + 100;
    double frqStep = (endFrq - frq) / (double)data.size();
    for (int i = 0; i < (int)data.size(); i++) {
        const qreal value = qSin(2 * M_PI * frq * qreal(sampleIndex % getFormat().sampleRate()) / getFormat().sampleRate());
        data[i] = value;
        sampleIndex++;
        frq += frqStep;
    }

    // TODO: when full, wait until empty (wait condition)
    m_buffer->writeData(data);

    emit newDataAvailable();
}

const QAudioFormat& AudioProducer::getFormat() const
{
    return m_format;
}

void AudioProducer::registered()
{
    // not implemented
}

void AudioProducer::unregistered()
{
    // not implemented
}
