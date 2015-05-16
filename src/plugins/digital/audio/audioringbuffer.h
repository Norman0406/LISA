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

#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H

#include <QIODevice>
#include <QAudioFormat>
#include <QMutex>
#include <QVector>
#include <QWaitCondition>

namespace Digital {
namespace Internal {

// TODO: implement multiple channels
class AudioRingBuffer
        : public QIODevice
{
    Q_OBJECT

public:
    AudioRingBuffer(const QAudioFormat& format, qint64 samples, QObject*);
    AudioRingBuffer(const QAudioFormat& format, double sec, QObject*);
    ~AudioRingBuffer();

    qint64 writeData(const char*, qint64);
    qint64 readData(char*, qint64);

    bool isEmpty() const;

    // TODO: write samples
    qint64 writeData(const QVector<double>&);

    qint64 getBuffer(QVector<double>&) const;
    qint64 getBuffer(QVector<double>&, qint64) const;
    qint64 getBufferLength() const;
    qint64 getBufferSize() const;

signals:
    void bytesRead(qint64);
    void bytesWritten(qint64);

private:
    const QAudioFormat m_format;
    QVector<double> m_buffer;
    qint64 m_bufferSize;
    qint64 m_bufferLength;
    qint64 m_position;

    mutable QMutex m_lock;
    QWaitCondition m_canWrite;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIORINGBUFFER_H
