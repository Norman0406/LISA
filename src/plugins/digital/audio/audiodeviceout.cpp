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

#include "audiodeviceout.h"
#include "audioproducer.h"
#include "audioproducerlist.h"

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QtEndian>

using namespace Digital::Internal;

AudioDeviceOut::AudioDeviceOut(QObject* parent, QAudioDeviceInfo deviceInfo)
    : AudioDevice(parent, deviceInfo),
      m_audioOutput(0),
      m_producerList(0)
{
}

AudioDeviceOut::~AudioDeviceOut()
{
}

void AudioDeviceOut::iInit(const QAudioDeviceInfo& info)
{
    m_producerList = new AudioProducerList(this);

    // create the input
    m_audioOutput = new QAudioOutput(info, getFormat(), this);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, &AudioDeviceOut::stateChanged);
}

void AudioDeviceOut::start()
{
    if (!m_producerList->isOpen())
        m_producerList->open(QIODevice::ReadOnly);

    // start
    m_audioOutput->start(m_producerList);
}

void AudioDeviceOut::stop()
{
    m_audioOutput->stop();

    if (m_producerList->isOpen())
        m_producerList->close();
}

void AudioDeviceOut::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

float AudioDeviceOut::getVolume() const
{
    return m_audioOutput->volume();
}

void AudioDeviceOut::registerProducer(AudioProducer* producer)
{
    producer->create(getFormat());
    m_producerList->add(producer);
}

void AudioDeviceOut::unregisterProducer(AudioProducer* producer)
{
    m_producerList->remove(producer);
}

void AudioDeviceOut::stateChanged(QAudio::State state)
{
    qDebug() << "state changed: " << state;

    switch (state) {
    case QAudio::ActiveState:
        break;
    case QAudio::SuspendedState:
        break;
    case QAudio::StoppedState:
        break;
    case QAudio::IdleState:
        break;
    }
}
