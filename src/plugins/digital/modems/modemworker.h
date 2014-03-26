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

#ifndef MODEMWORKER_H
#define MODEMWORKER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QWaitCondition>
#include <QAudioFormat>

namespace Digital {
namespace Internal {

class AudioDeviceIn;
class AudioDeviceOut;
class Modem;
class ModemReceiver;
class ModemTransmitter;

class ModemWorker
        : public QObject
{
    Q_OBJECT

public:
    ModemWorker();
    ~ModemWorker();

    bool create(QString);
    bool init(const QAudioFormat&);
    bool shutdown();
    void terminate();
    void inputBlock(const QVector<double>&);
    void send(QString);

    Modem* getModem();

public slots:
    void initReceiver(AudioDeviceIn*);
    void initTransmitter(AudioDeviceOut*);
    void run();

signals:
    void received(char);
    void finished();

private:
    Modem* m_modem;
    ModemReceiver* m_receiver;
    ModemTransmitter* m_transmitter;
    bool m_terminate;

    QMutex m_mutex;
    QWaitCondition m_bufferEmpty;
    QWaitCondition m_bufferFull;
    QVector<double> m_buffer;
    bool m_isBufferEmpty;
};

} // namespace Internal
} // namespace Digital

#endif // MODEMWORKER_H
