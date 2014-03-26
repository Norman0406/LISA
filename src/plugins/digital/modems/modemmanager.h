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

#ifndef MODEMMANAGER_H
#define MODEMMANAGER_H

#include <QThread>
#include <QMutex>
#include "../audio/audioconsumer.h"
#include "modem.h"

namespace Digital {
namespace Internal {

class AudioDeviceIn;
class AudioDeviceOut;
class ModemWorker;

class ModemManager
        : public QObject
{
    Q_OBJECT

public:
    ModemManager(QObject*);
    ~ModemManager();

    void inDeviceReady(AudioDeviceIn*);
    void outDeviceReady(AudioDeviceOut*);

    int create(QString);    // creates and initializes a new modem
    bool terminate(int);    // completely removes the modem
    void terminateAll();    // completely removes all modems

    bool start(int);        // begins modem processing
    bool stop(int);         // pauses modem processing
    bool shutdown(int);     // deinitializes the modem

    void startAll();
    void stopAll();
    void shutdownAll();

public slots:
    void frequencySelected(double);

private slots:
    void receivedChar(char);
    void modemFinished();

signals:
    void received(int, char);
    void frequencyChanged(double);
    void bandwidthChanged(double);
    void initReceiver(AudioDeviceIn*);
    void initTransmitter(AudioDeviceOut*);

protected:
    /*void registered();
    void unregistered();
    void audioDataReady(const QVector<double>&);*/

private:
    enum ModemState {
        MODEMSTATE_RUNNING,
        MODEMSTATE_PAUSED,
        MODEMSTATE_ISTERMINATING,
    };

    AudioDeviceIn*      m_inputDevice;
    AudioDeviceOut*     m_outputDevice;
    QMutex              m_finishedMutex;
    QList<ModemWorker*> m_modems;
    QList<QThread*>     m_modemThreads;
    QList<ModemState>   m_modemStates;
};

} // namespace Internal
} // namespace Digital

#endif // MODEMMANAGER_H
