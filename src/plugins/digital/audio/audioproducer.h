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

#ifndef AUDIOPRODUCER_H
#define AUDIOPRODUCER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include "audioringbuffer.h"

namespace Digital {
namespace Internal {

class AudioProducerList;

class AudioProducer
        : public QObject
{
    Q_OBJECT

    friend class AudioProducerList;

public:
    AudioProducer(QObject* parent, double);
    ~AudioProducer();

    virtual void create(QAudioFormat, AudioProducerList*);

    void write(QVector<double>&);
    void flush();

    AudioRingBuffer* m_buffer;

signals:
    void newDataAvailable();

private slots:
    void bytesRead(qint64);

protected:
    const QAudioFormat& getFormat() const;

    virtual void registered();
    virtual void unregistered();

private:
    AudioProducerList* m_producerList;
    QAudioFormat m_format;
    double m_bufferLengthSec;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIOPRODUCER_H
