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

#ifndef AUDIOCONSUMERLIST_H
#define AUDIOCONSUMERLIST_H

#include <QIODevice>

namespace Digital {
namespace Internal {

class AudioConsumer;

/**
 * @brief The AudioConsumerList class is a QIODevice that can be used as input device for
 * QAudioInput. Any class that is derived from AudioConsumer can register itself to the
 * consumer list and will from now on receive data based on the consumer specialization
 * (i.e. continous data or latest data).
 */
class AudioConsumerList
        : public QIODevice
{
public:
    AudioConsumerList(QObject* parent);
    ~AudioConsumerList();

    void add(AudioConsumer*);
    void remove(AudioConsumer*);

protected:
    qint64 writeData(const char* data, qint64 len);
    qint64 readData(char* data, qint64 maxlen);

private:
    QList<AudioConsumer*> m_consumerList;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIOCONSUMERLIST_H
