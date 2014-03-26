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
#include "audioproducerlist.h"

using namespace Digital::Internal;

AudioProducer::AudioProducer(QObject* parent, double length)
    : QObject(parent),
      m_bufferLengthSec(length),
      m_producerList(0)
{
}

AudioProducer::~AudioProducer()
{
}

void AudioProducer::create(QAudioFormat format, AudioProducerList* producerList)
{
    m_format = format;
    m_producerList = producerList;
    m_buffer = new AudioRingBuffer(m_format, m_bufferLengthSec, this);
}

#include <QDebug>

void AudioProducer::write(QVector<double>& data)
{
    qint64 written = m_buffer->writeData(data);
    Q_UNUSED(written);
    emit newDataAvailable();
}

const QAudioFormat& AudioProducer::getFormat() const
{
    return m_format;
}

void AudioProducer::registered()
{
    // not implemented
}

void AudioProducer::unregistered()
{
    // not implemented
}
