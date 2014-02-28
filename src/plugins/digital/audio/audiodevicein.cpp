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

#include "audiodevicein.h"
#include "audioconsumer.h"
#include "audioconsumerlist.h"

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QtEndian>

using namespace Digital::Internal;

AudioDeviceIn::AudioDeviceIn(QObject* parent, QAudioDeviceInfo deviceInfo)
    : AudioDevice(parent, deviceInfo),
    m_audioInput(0),
    m_consumerList(0)
{
}

AudioDeviceIn::~AudioDeviceIn()
{
}

void AudioDeviceIn::iInit(const QAudioDeviceInfo& info)
{
    m_consumerList = new AudioConsumerList(this);

    // create the input
    m_audioInput = new QAudioInput(info, getFormat(), this);
    connect(m_audioInput, &QAudioInput::stateChanged, this, &AudioDeviceIn::stateChanged);
}

void AudioDeviceIn::start()
{
    if (!m_consumerList->isOpen())
        m_consumerList->open(QIODevice::WriteOnly);

    // start
    m_audioInput->start(m_consumerList);
}

void AudioDeviceIn::stop()
{
    m_audioInput->stop();

    if (m_consumerList->isOpen())
        m_consumerList->close();
}

void AudioDeviceIn::setVolume(float volume)
{
    m_audioInput->setVolume(volume);
}

float AudioDeviceIn::getVolume() const
{
    return m_audioInput->volume();
}

void AudioDeviceIn::registerConsumer(AudioConsumer* consumer)
{
    consumer->create(getFormat());
    m_consumerList->add(consumer);
}

void AudioDeviceIn::unregisterConsumer(AudioConsumer* consumer)
{
    m_consumerList->remove(consumer);
}

void AudioDeviceIn::stateChanged(QAudio::State state)
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
