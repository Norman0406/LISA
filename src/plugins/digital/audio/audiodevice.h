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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QAudioInput>
#include <QtEndian>

namespace Digital {
namespace Internal {

// TODO: run inside a thread

class AudioDevice
        : public QObject
{
    Q_OBJECT

public:
    virtual ~AudioDevice();

    const QString& getDeviceName() const;

    void setFormat(QAudioFormat);
    void setSampleRate(int);
    void setSampleSize(int);
    void setSampleType(QAudioFormat::SampleType);
    void setChannelCount(int);

    const QAudioFormat& getFormat() const;
    int getSampleRate() const;
    int getSampleSize() const;
    QAudioFormat::SampleType getSampleType() const;
    int getChannelCount() const;
    int getFrameSize() const;

    void init();

    bool isReady() const;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setVolume(float) = 0;
    virtual float getVolume() const = 0;

    qreal pcmToReal(const char*);
    static qreal pcmToReal(const QAudioFormat& format, const char* ptr);
    static qint64 audioLength(const QAudioFormat& format, qint64 microSeconds);

    template <typename T>
    static T convertByteOrder(const QAudioFormat::Endian& endian, const unsigned char* ptr);

signals:
    void dataReady(const QAudioFormat&, QByteArray& buffer, int dataLength);

protected:
    AudioDevice(QObject* parent, QAudioDeviceInfo deviceInfo);

    virtual void iInit(const QAudioDeviceInfo&) = 0;

private:
    bool m_ready;
    QAudioFormat m_format;
    QAudioDeviceInfo m_deviceInfo;
    QString m_deviceName;
    QList<QAudioFormat::Endian> m_byteOrders;
    QList<int> m_channelCounts;
    QStringList m_codecs;
    QList<int> m_sampleRates;
    QList<int> m_sampleSizes;
    QList<QAudioFormat::SampleType> m_sampleTypes;
};

template <typename T>
T AudioDevice::convertByteOrder(const QAudioFormat::Endian& endian, const unsigned char* ptr)
{
    if (endian == QAudioFormat::BigEndian)
        return qFromBigEndian<T>(ptr);
    else
        return qFromLittleEndian<T>(ptr);
}

} // namespace Internal
} // namespace Digital

#endif // AUDIODEVICE_H
