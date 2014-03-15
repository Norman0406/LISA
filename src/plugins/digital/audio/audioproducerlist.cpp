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
#include "audiodevice.h"

#include <QDebug>

using namespace Digital::Internal;

AudioProducerList::AudioProducerList(AudioDevice* parent)
    : QIODevice(parent),
      m_device(parent)
{

}

AudioProducerList::~AudioProducerList()
{

}

void AudioProducerList::add(AudioProducer* producer)
{
    m_producerList.push_back(producer);
    producer->registered();
}

void AudioProducerList::remove(AudioProducer* producer)
{
    m_producerList.removeAll(producer);
    producer->unregistered();
}

qint64 AudioProducerList::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    qDebug() << "writing to audio producer list is not supported";
    return 0;
}

qint64 AudioProducerList::readData(char* data, qint64 maxlen)
{
    // TODO: add up read data such that multiple signals can be
    // added on the same channel

    // write data into each registered audio consumer
    qint64 bytesRead = 0;
    foreach (AudioProducer* producer, m_producerList) {
        qint64 read = producer->readData(data, maxlen);
        bytesRead = qMax(bytesRead, read);
    }

    return bytesRead;
}
