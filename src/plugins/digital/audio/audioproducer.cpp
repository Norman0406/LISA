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

using namespace Digital::Internal;

AudioProducer::AudioProducer(QObject* parent)
    : QObject(parent)
{
}

AudioProducer::~AudioProducer()
{
}


void AudioProducer::create(QAudioFormat format)
{
    m_format = format;
}

const QAudioFormat& AudioProducer::getFormat() const
{
    return m_format;
}

#include "audiodevice.h"
#include <QtMath>

const double frq = 1000;
int sampleIndex = 0;

qint64 AudioProducer::readData(char* data, qint64 len)
{
    const int bytesPerSample = (getFormat().sampleSize() / 8) * getFormat().channelCount();

    if (m_format.isValid()) {
        qint64 bytesReadTotal = 0;

        qint64 bytesLeft = len;
        while (bytesLeft > 0) {
            const qreal value = qSin(2 * M_PI * frq * qreal(sampleIndex % getFormat().sampleRate()) / getFormat().sampleRate());
            AudioDevice::realToPcm(getFormat(), value, data + bytesReadTotal);

            bytesReadTotal += bytesPerSample;
            bytesLeft -= bytesPerSample;
            sampleIndex++;
        };

        return bytesReadTotal;
    }

    return 0;
}

void AudioProducer::registered()
{
    // not implemented
}

void AudioProducer::unregistered()
{
    // not implemented
}
