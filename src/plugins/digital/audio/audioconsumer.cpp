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

#include "audioconsumer.h"
#include "audiodevice.h"

using namespace Digital::Internal;

AudioConsumer::AudioConsumer(qint64 samples, QObject* parent)
    : QObject(parent),
      m_samples(samples),
      m_position(0)
{
    setNumSamples(samples);
}

AudioConsumer::~AudioConsumer()
{
}

void AudioConsumer::setNumSamples(qint64 samples)
{
    m_samples = samples;
    m_position = 0;
    m_realData.resize(m_samples);
    m_realData.fill(0);
}

qint64 AudioConsumer::getNumSamples() const
{
    return m_samples;
}

void AudioConsumer::create(QAudioFormat format)
{
    m_format = format;
}

const QAudioFormat& AudioConsumer::getFormat() const
{
    return m_format;
}

qint64 AudioConsumer::writeData(const char* data, qint64 len)
{
    const int bytesPerSample = (getFormat().sampleSize() / 8) * getFormat().channelCount();

    if (getFormat().isValid()) {
        qint64 bytesWrittenTotal = 0;

        // signal that a block of data is available as soon as the buffer is full
        qint64 bytesLeft = len;
        do {
            qreal pcmSample = AudioDevice::pcmToReal(getFormat(), data + bytesWrittenTotal);
            m_realData[m_position] = pcmSample;
            m_position++;

            // the buffer is full, convert the data block into real data
            if (m_position == m_realData.size()) {
                m_position = 0;

                audioDataReady(m_realData);
            }

            bytesWrittenTotal += bytesPerSample;
            bytesLeft -= bytesPerSample;
        } while (bytesLeft > 0);

        return bytesWrittenTotal;
    }

    return 0;
}

void AudioConsumer::registered()
{
}

void AudioConsumer::unregistered()
{
}
