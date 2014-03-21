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
    m_bufferSize = format.channelCount() * format.sampleRate() * sec;

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
    //const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();
    qint64 samplesWrittenTotal = 0;

    qint64 samplesLeft = data.size();
    m_lock.lock();
    while (samplesLeft > 0) {
        m_buffer[m_position] = data[samplesWrittenTotal];
        m_position = (m_position + 1) % m_buffer.size();

        samplesWrittenTotal++;
        samplesLeft--;
    };

    m_bufferLength += samplesWrittenTotal;
    if (m_bufferLength > m_bufferSize)
        m_bufferLength = m_bufferSize;
    m_lock.unlock();

    //emit bytesWritten(samplesWrittenTotal * bytesPerSample);
    //emit readyRead();

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
    while (bytesLeft > 0) {
        qreal pcmSample = AudioDevice::pcmToReal(m_format, data + bytesWrittenTotal);
        m_buffer[m_position] = pcmSample;
        m_position = (m_position + 1) % m_buffer.size();

        bytesWrittenTotal += bytesPerSample;
        samplesWrittenTotal++;
        bytesLeft -= bytesPerSample;
    };

    m_bufferLength += samplesWrittenTotal;
    if (m_bufferLength > m_bufferSize)
        m_bufferLength = m_bufferSize;
    m_lock.unlock();

    //emit bytesWritten(bytesWrittenTotal);

    return bytesWrittenTotal;
}

bool AudioRingBuffer::isEmpty() const
{
    return m_bufferLength == 0;
}

qint64 AudioRingBuffer::readData(char* data, qint64 maxSize)
{
    const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();

    qint64 bytesToRead = qMin(maxSize, m_bufferLength * bytesPerSample);

    qint64 bytesReadTotal = 0;
    qint64 samplesReadTotal = 0;

    m_lock.lock();
    qint64 bytesLeft = bytesToRead;
    while (bytesLeft > 0) {
        int position = (m_position - m_bufferLength + m_buffer.size()) % m_buffer.size();
        const double& value = m_buffer[position];
        AudioDevice::realToPcm(m_format, value, data + bytesReadTotal);

        m_bufferLength--;

        bytesReadTotal += bytesPerSample;
        samplesReadTotal++;
        bytesLeft -= bytesPerSample;
    };

    if (m_bufferLength < 0)
        m_bufferLength = 0;

    // reset position pointer
    m_position = (m_position - m_bufferLength + m_buffer.size()) % m_buffer.size();
    m_lock.unlock();

    return bytesReadTotal;
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

qint64 AudioRingBuffer::getBufferSize() const
{
    return m_bufferSize;
}
