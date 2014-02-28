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

#include "audioconsumerlist.h"
#include "audioconsumer.h"

#include <QDebug>

using namespace Digital::Internal;

AudioConsumerList::AudioConsumerList(QObject* parent)
    : QIODevice(parent)
{
}

AudioConsumerList::~AudioConsumerList()
{
}

void AudioConsumerList::add(AudioConsumer* consumer)
{
    // register new audio consumer
    m_consumerList.push_back(consumer);
    consumer->registered();
}

void AudioConsumerList::remove(AudioConsumer* consumer)
{
    // remove existing audio consumer
    m_consumerList.removeAll(consumer);
    consumer->unregistered();
}

qint64 AudioConsumerList::writeData(const char* data, qint64 len)
{
    // write data into each registered audio consumer
    qint64 bytesWritten = 0;
    foreach (AudioConsumer* consumer, m_consumerList) {
        qint64 written = consumer->writeData(data, len);
        bytesWritten = qMax(bytesWritten, written);
    }

    return bytesWritten;
}

qint64 AudioConsumerList::readData(char* data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    qDebug() << "reading from audio consumer list is not implemented";
    return 0;
}
