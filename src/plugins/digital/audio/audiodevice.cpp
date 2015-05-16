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

#include "audiodevice.h"

#include <QtDebug>
#include <QAudioDeviceInfo>
#include <QAudioInput>

using namespace Digital::Internal;

AudioDevice::AudioDevice(QObject* parent, QAudioDeviceInfo deviceInfo)
    : QObject(parent),
      m_ready(false)
{
    m_deviceInfo = deviceInfo;
    m_deviceName = deviceInfo.deviceName();
    m_byteOrders = deviceInfo.supportedByteOrders();
    m_channelCounts = deviceInfo.supportedChannelCounts();
    m_codecs = deviceInfo.supportedCodecs();
    m_sampleRates = deviceInfo.supportedSampleRates();
    m_sampleSizes = deviceInfo.supportedSampleSizes();
    m_sampleTypes = deviceInfo.supportedSampleTypes();

    // choose default format
    m_format = m_deviceInfo.preferredFormat();
}

AudioDevice::~AudioDevice()
{
}

const QString& AudioDevice::getDeviceName() const
{
    return m_deviceName;
}

void AudioDevice::setFormat(QAudioFormat format)
{
    m_format = format;
}

void AudioDevice::setSampleRate(int sampleRate)
{
    m_format.setSampleRate(sampleRate);
}

void AudioDevice::setSampleSize(int sampleSize)
{
    m_format.setSampleSize(sampleSize);
}

void AudioDevice::setSampleType(QAudioFormat::SampleType sampleType)
{
    m_format.setSampleType(sampleType);
}

void AudioDevice::setChannelCount(int channelCount)
{
    m_format.setChannelCount(channelCount);
}

const QAudioFormat& AudioDevice::getFormat() const
{
    return m_format;
}

int AudioDevice::getSampleRate() const
{
    return m_format.sampleRate();
}

int AudioDevice::getSampleSize() const
{
    return m_format.sampleSize();
}

QAudioFormat::SampleType AudioDevice::getSampleType() const
{
    return m_format.sampleType();
}

int AudioDevice::getChannelCount() const
{
    return m_format.channelCount();
}

int AudioDevice::getFrameSize() const
{
    return (m_format.sampleSize() / 8) * m_format.channelCount();
}

void AudioDevice::init()
{
    if (!m_deviceInfo.isFormatSupported(m_format)) {
        qWarning() << "format not supported, choosing nearest format.";
        m_format = m_deviceInfo.nearestFormat(m_format);
    }

    iInit(m_deviceInfo);

    m_ready = true;
}

bool AudioDevice::isReady() const
{
    return m_ready;
}

qreal AudioDevice::pcmToReal(const char* ptr)
{
    return pcmToReal(m_format, ptr);
}

qreal AudioDevice::pcmToReal(const QAudioFormat& format, const char* data)
{
    qreal realValue = 0;

    if (format.codec() != QString::fromLatin1("audio/pcm")) {
        qCritical() << "format is not pcm";
        return 0;
    }

    if (!data) {
        qCritical() << "invalid data";
        return 0;
    }

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
    if (format.sampleSize() == 8) {
        if (format.sampleType() == QAudioFormat::UnSignedInt) {
            quint8 value = convertByteOrder<quint8>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(255); // [0, 1]
            realValue = 2 * realValue - 1;  // [-1, 1]
        }
        else if (format.sampleType() == QAudioFormat::SignedInt) {
            qint8 value = convertByteOrder<qint8>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(128); // [-1, 1]
        }
        else {
            qCritical() << "unsupported sample type";
            return 0;
        }
    }
    else if (format.sampleSize() == 16) {
        if (format.sampleType() == QAudioFormat::UnSignedInt) {
            quint16 value = convertByteOrder<quint8>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(65535); // [0, 1]
            realValue = 2 * realValue - 1;  // [-1, 1]
        }
        else if (format.sampleType() == QAudioFormat::SignedInt) {
            qint16 value = convertByteOrder<qint16>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(32767); // [-1, 1]
        }
        else {
            qCritical() << "unsupported sample type";
            return 0;
        }
    }
    else if (format.sampleSize() == 32) {
        if (format.sampleType() == QAudioFormat::UnSignedInt) {
            quint32 value = convertByteOrder<quint32>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(0xffffffff); // [0, 1]
            realValue = 2 * realValue - 1;  // [-1, 1]
        }
        else if (format.sampleType() == QAudioFormat::SignedInt) {
            qint32 value = convertByteOrder<qint32>(format.byteOrder(), ptr);
            realValue = qreal(value) / qreal(0x7fffffff); // [-1, 1]
        }
        else if (format.sampleType() == QAudioFormat::Float) {
            // UNDONE: not sure if correct
            realValue = *reinterpret_cast<const qreal*>(ptr);
            realValue /= qreal(0x7fffffff);
        }
        else {
            qCritical() << "unsupported sample type";
            return 0;
        }
    }
    else {
        qCritical() << "unknown sample size: " << format.sampleSize();
        return 0;
    }

    return realValue;
}

void AudioDevice::realToPcm(const QAudioFormat& format, qreal real, char* data)
{
    QString codec = QLatin1String("audio/pcm");
    if (format.codec() != codec) {
        qCritical() << "format is not pcm";
        return;
    }

    if (!data) {
        qCritical() << "invalid data";
        return;
    }

    // UNDONE
    if (format.sampleSize() == 16) {
        if (format.sampleType() == QAudioFormat::UnSignedInt) {
            quint16 value = (quint16)(((real + 1) / 2.0) * qreal(65535));
            *(quint16*)data = value;
        }
        else if (format.sampleType() == QAudioFormat::SignedInt) {
            qint16 value = (qint16)(real * qreal(32767));
            *(quint16*)data = value;
        }
        else {
            qCritical() << "unsupported sample type";
            return;
        }
    }
}

qint64 AudioDevice::audioLength(const QAudioFormat& format, qint64 microSeconds)
{
   qint64 result = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
       * microSeconds / 1000000;
   result -= result % (format.channelCount() * format.sampleSize());
   return result;
}
