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

#include "audioringbuffer.h"
#include "audiodevice.h"

using namespace Digital::Internal;

AudioRingBuffer::AudioRingBuffer(const QAudioFormat& format, qint64 samples, QObject* parent)
    : QIODevice(parent),
      m_format(format)
{
    m_bufferSize = samples;

    m_position = 0;
    m_bufferLength = 0;

    m_buffer.resize(m_bufferSize);
    m_buffer.fill(0);
    open(QIODevice::ReadWrite);
}

AudioRingBuffer::AudioRingBuffer(const QAudioFormat& format, double sec, QObject* parent)
    : QIODevice(parent),
      m_format(format)
{
    m_bufferSize = format.channelCount() * (format.sampleSize() / 8) *
            (format.sampleRate()) * sec;

    m_position = 0;
    m_bufferLength = 0;

    m_buffer.resize(m_bufferSize);
    m_buffer.fill(0);
    open(QIODevice::ReadWrite);
}

AudioRingBuffer::~AudioRingBuffer()
{
    close();
}

qint64 AudioRingBuffer::writeData(const QVector<double>& data)
{
    qint64 samplesWrittenTotal = 0;

    qint64 samplesLeft = data.size();
    do {
        m_buffer[m_position] = data[samplesWrittenTotal];
        m_position = (m_position + 1) % m_buffer.size();

        samplesWrittenTotal++;
        samplesLeft--;
    } while (samplesLeft > 0);

    m_bufferLength += samplesWrittenTotal;
    if (m_bufferLength > m_bufferSize)
        m_bufferLength = m_bufferSize;

    return samplesWrittenTotal;
}

qint64 AudioRingBuffer::writeData(const char* data, qint64 len)
{
    const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();

    qint64 bytesWrittenTotal = 0;
    qint64 samplesWrittenTotal = 0;

    // signal that a block of data is available as soon as the buffer is full
    m_lock.lock();
    qint64 bytesLeft = len;
    do {
        qreal pcmSample = AudioDevice::pcmToReal(m_format, data + bytesWrittenTotal);
        m_buffer[m_position] = pcmSample;
        m_position = (m_position + 1) % m_buffer.size();

        bytesWrittenTotal += bytesPerSample;
        samplesWrittenTotal++;
        bytesLeft -= bytesPerSample;
    } while (bytesLeft > 0);
    m_lock.unlock();

    m_bufferLength += samplesWrittenTotal;
    if (m_bufferLength > m_bufferSize)
        m_bufferLength = m_bufferSize;

    return bytesWrittenTotal;
}

qint64 AudioRingBuffer::readData(char* data, qint64 maxSize)
{
    // UNDONE
    return 0;
}

qint64 AudioRingBuffer::getBuffer(QVector<double>& buffer) const
{
    return getBuffer(buffer, m_bufferSize);
}

qint64 AudioRingBuffer::getBuffer(QVector<double>& buffer, qint64 size) const
{
    m_lock.lock();
    buffer.resize(m_buffer.size());

    size = size > m_bufferSize ? m_bufferSize : size;

    for (int i = m_position, j = 0; j < size; i = (i + 1) % m_bufferSize, j++)
        buffer[j] = m_buffer[i];

    m_lock.unlock();
    return size;
}

qint64 AudioRingBuffer::getBufferLength() const
{
    return m_bufferLength;
}
