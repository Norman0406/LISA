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

#include "audioproducerlist.h"
#include "audioproducer.h"
#include "audiodeviceout.h"

#include <QDebug>

using namespace Digital::Internal;

AudioProducerList::AudioProducerList(AudioDeviceOut* device)
    : QIODevice(device),
      m_device(device)
{
}

AudioProducerList::~AudioProducerList()
{

}

void AudioProducerList::add(AudioProducer* producer)
{
    connect(producer, &AudioProducer::newDataAvailable, this, &AudioProducerList::requestSoundcard);
    m_producerList.push_back(producer);
    producer->registered();
}

void AudioProducerList::remove(AudioProducer* producer)
{
    m_producerList.removeAll(producer);
    producer->unregistered();
}

void AudioProducerList::requestSoundcard()
{
    m_mutex.lock();
    if (m_device && !m_device->isOpen()) {
        m_device->start();
    }
    m_mutex.unlock();
}

void AudioProducerList::bytesWritten(qint64 bytes)
{
    qDebug() << "written " << bytes;
}

qint64 AudioProducerList::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    qDebug() << "writing to audio consumer list is not supported";
    return 0;
}

#include <cmath>
qint64 AudioProducerList::readData(char* data, qint64 maxlen)
{
    const QAudioFormat& format = m_device->getFormat();
    const int bytesPerSample = format.channelCount() * format.sampleSize() / 8;

    int bytesToRead = qMin(m_device->getDevice()->periodSize(), (int)maxlen);
    int numSamples = bytesToRead / bytesPerSample;

    // the sample buffer that will contain the mixed audio chunk
    QVector<qreal> sampleBuffer(numSamples);
    sampleBuffer.fill(0);

    // stores the number of currently active producers for later normalization
    QVector<int> activeProducers(numSamples);
    activeProducers.fill(0);

    char* localBuffer = new char[bytesToRead];

    qint64 bytesRead = 0;

    // read data from all producers
    foreach (AudioProducer* producer, m_producerList) {
        qint64 bytesReadLocal = producer->m_buffer->read(localBuffer, bytesToRead);
        int numLocalSamples = bytesReadLocal / bytesPerSample;

        bytesRead = qMax(bytesRead, bytesReadLocal);

        for (int i = 0; i < numLocalSamples; i++) {
            qreal value = AudioDevice::pcmToReal(format, localBuffer + i * bytesPerSample);
            sampleBuffer[i] += value;
            activeProducers[i]++;
        }
    }
    delete[] localBuffer;

    //qDebug() << bytesRead;

    int samples = bytesRead / bytesPerSample;

    // TODO: find a better mixing technique for multiple audio streams

    // mix samples and write to output buffer
    for (int i = 0; i < samples; i++) {
        qreal value = sampleBuffer[i];
        double normalization = (double)activeProducers[i];;
        AudioDevice::realToPcm(format, value / normalization, data + i * bytesPerSample);
    }

    // TODO: wait for empty buffer and stop the device then
    /*if (bytesRead == 0)
        m_device->stop();*/

    return bytesRead;
}
