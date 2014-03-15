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

#ifndef AUDIOPRODUCERLIST_H
#define AUDIOPRODUCERLIST_H

#include <QIODevice>

namespace Digital {
namespace Internal {

class AudioProducer;

class AudioProducerList
        : public QIODevice
{
public:
    AudioProducerList(QObject* parent);
    ~AudioProducerList();

    void add(AudioProducer*);
    void remove(AudioProducer*);

protected:
    qint64 writeData(const char* data, qint64 len);
    qint64 readData(char* data, qint64 maxlen);

private:
    QList<AudioProducer*> m_producerList;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIOPRODUCERLIST_H
