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

#include <QThread>
#include <QDebug>
qint64 AudioRingBuffer::writeData(const QVector<double>& data)
{
    m_lock.lock();
    //this->thread()->msleep(10);

    qDebug() << "buffer length: " << m_bufferLength;
    // UNDONE: FFT still writes data on the gui thread
    /*while (m_bufferLength == m_bufferSize)
        m_canWrite.wait(&m_lock);*/

    qint64 samplesWrittenTotal = 0;

    qint64 samplesLeft = data.size();
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

    const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();
    emit bytesWritten(samplesWrittenTotal * bytesPerSample);

    return samplesWrittenTotal;
}

qint64 AudioRingBuffer::writeData(const char* data, qint64 len)
{
    m_lock.lock();

    /*while (m_bufferLength == m_bufferSize)
        m_canWrite.wait(&m_lock);*/
    const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();

    qint64 bytesWrittenTotal = 0;
    qint64 samplesWrittenTotal = 0;

    // signal that a block of data is available as soon as the buffer is full
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

    emit bytesWritten(bytesWrittenTotal);

    return bytesWrittenTotal;
}

bool AudioRingBuffer::isEmpty() const
{
    return m_bufferLength == 0;
}

qint64 AudioRingBuffer::readData(char* data, qint64 maxSize)
{
    m_lock.lock();

    const int bytesPerSample = (m_format.sampleSize() / 8) * m_format.channelCount();

    qint64 bytesToRead = qMin(maxSize, m_bufferLength * bytesPerSample);

    qint64 bytesReadTotal = 0;
    qint64 samplesReadTotal = 0;

    qint64 bytesLeft = bytesToRead;
    int position = (m_position - m_bufferLength + m_buffer.size()) % m_buffer.size();
    while (bytesLeft > 0) {
        const double& value = m_buffer[position];
        AudioDevice::realToPcm(m_format, value, data + bytesReadTotal);

        position = (position + 1) % m_buffer.size();
        bytesReadTotal += bytesPerSample;
        samplesReadTotal++;
        bytesLeft -= bytesPerSample;
    };

    m_bufferLength -= samplesReadTotal;
    if (m_bufferLength < 0)
        m_bufferLength = 0;

    m_canWrite.wakeAll();

    m_lock.unlock();

    emit bytesRead(bytesReadTotal);

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
