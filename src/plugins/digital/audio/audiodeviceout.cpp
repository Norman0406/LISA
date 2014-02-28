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

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QtEndian>

using namespace Digital::Internal;

AudioDeviceOut::AudioDeviceOut(QObject* parent, QAudioDeviceInfo deviceInfo)
    : AudioDevice(parent, deviceInfo),
      m_audioOutput(0),
      m_audioBuffer(0)
{
}

AudioDeviceOut::~AudioDeviceOut()
{
}

void AudioDeviceOut::iInit(const QAudioDeviceInfo& info)
{
    // create audio buffer
    const double bufferLengthSec = 1.0;
    m_audioBuffer = new AudioRingBuffer(getFormat(), bufferLengthSec, this);

    // create the input
    m_audioOutput = new QAudioOutput(info, getFormat(), this);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, &AudioDeviceOut::stateChanged);
}

void AudioDeviceOut::start()
{
    if (!m_audioBuffer->isOpen())
        m_audioBuffer->open(QIODevice::ReadWrite);

    m_audioOutput->start(m_audioBuffer);
}

void AudioDeviceOut::stop()
{
    m_audioOutput->stop();

    if (m_audioBuffer->isOpen())
        m_audioBuffer->close();
}

void AudioDeviceOut::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

float AudioDeviceOut::getVolume() const
{
    return m_audioOutput->volume();
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

AudioRingBuffer* AudioDeviceOut::getBuffer() const
{
    return m_audioBuffer;
}
