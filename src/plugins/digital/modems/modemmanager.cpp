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

#include "modemmanager.h"
#include "modemworker.h"
#include "modemrtty.h"
#include "../factory.h"
#include "../audio/audiodevice.h"
#include "../audio/audiodevicein.h"
#include "../audio/audiodeviceout.h"

#include <QDebug>

using namespace Digital::Internal;

ModemManager::ModemManager(QObject* parent)
    : QObject(parent),
      m_inputDevice(0),
      m_outputDevice(0)
    //: AudioConsumer(512, parent)
{
}

ModemManager::~ModemManager()
{
    terminateAll();
    foreach(QThread* thread, m_modemThreads) {
        thread->quit();
        thread->wait();
    }
}

/*void ModemManager::registered()
{
    foreach (ModemWorker* worker, m_modems) {
        worker->init(getFormat());
    }
}

void ModemManager::unregistered()
{
    shutdownAll();
}*/

/*void ModemManager::audioDataReady(const QVector<double>& data)
{
    m_finishedMutex.lock();
    // process active modem workers
    for (int i = 0; i < (int)m_modems.size(); i++) {
        if (m_modemStates[i] == MODEMSTATE_RUNNING) {
            m_modems[i]->inputBlock(data);
        }
    }
    m_finishedMutex.unlock();
}*/


void ModemManager::inDeviceReady(AudioDeviceIn* inputDevice)
{
    if (!m_inputDevice) {
        m_inputDevice = inputDevice;
    }

    emit initReceiver(inputDevice);
}

void ModemManager::outDeviceReady(AudioDeviceOut* outputDevice)
{
    if (!m_outputDevice) {
        m_outputDevice = outputDevice;
    }

    emit initTransmitter(outputDevice);
}

int ModemManager::create(QString type)
{
    // create new worker with the specified modem type
    ModemWorker* worker = new ModemWorker();

    connect(this, &ModemManager::initReceiver, worker, &ModemWorker::initReceiver);
    connect(this, &ModemManager::initTransmitter, worker, &ModemWorker::initTransmitter);

    if (worker->create(type)) {
        connect(worker->getModem(), &Modem::frequencyChanged, this, &ModemManager::frequencyChanged);
        connect(worker->getModem(), &Modem::bandwidthChanged, this, &ModemManager::bandwidthChanged);

        // Try to init the modem. This can fail if create() is called before the soundcard
        // is available. In this case, init() will be called again in registered().
        //worker->init();

        // TODO: maybe also use signal/slots instead of a direct call?
        if (m_inputDevice) {
            worker->initReceiver(m_inputDevice);
        }

        if (m_outputDevice) {
            worker->initTransmitter(m_outputDevice);
        }

        // create a new thread for the worker class
        QThread* thread = new QThread(this);
        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &ModemWorker::run);
        connect(worker, &ModemWorker::finished, thread, &QThread::quit);
        connect(worker, &ModemWorker::finished, worker, &ModemWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, this, &ModemManager::modemFinished);

        connect(worker, &ModemWorker::received, this, &ModemManager::receivedChar);

        m_modems.push_back(worker);
        m_modemThreads.push_back(thread);
        m_modemStates.push_back(MODEMSTATE_PAUSED);

        // Although the thread is already started here, it is not automatically decoding.
        // The decoding process is bound to the input blocks and decoding stops when no
        // data blocks are delivered.
        thread->start();

        return m_modems.size() - 1;
    }
    else
        delete worker;

    return -1;
}

bool ModemManager::terminate(int index)
{
    if (index < 0 || index >= m_modems.size())
        return false;

    // Only signals terminate for the worker. The thread is actually terminated when all work
    // is done and modemFinished() is called
    m_modems[index]->terminate();

    return true;
}

void ModemManager::terminateAll()
{
    for (int i = 0; i < (int)m_modems.size(); i++) {
        m_modemStates[i] = MODEMSTATE_ISTERMINATING;
        m_modems[i]->terminate();
    }
}

void ModemManager::modemFinished()
{
    m_finishedMutex.lock();
    QThread* sender = qobject_cast<QThread*>(QObject::sender());
    int index = m_modemThreads.indexOf(sender);
    if (index >= 0 && index < m_modems.size()) {
        m_modems.removeAt(index);
        m_modemThreads.removeAt(index);
        m_modemStates.removeAt(index);
    }
    m_finishedMutex.unlock();
}

void ModemManager::receivedChar(char character)
{
    // find the sending object and emit the received signal,
    // indicating the modem index
    ModemWorker* sender = qobject_cast<ModemWorker*>(QObject::sender());
    for (int i = 0; i < (int)m_modems.size(); i++) {
        if (m_modems[i] == sender)
            emit received(i, character);
    }
}

bool ModemManager::start(int index)
{
    if (index < 0 || index >= m_modemStates.size())
        return false;

    m_modemStates[index] = MODEMSTATE_RUNNING;

    return true;
}

bool ModemManager::stop(int index)
{
    if (index < 0 || index >= m_modemStates.size())
        return false;

    m_modemStates[index] = MODEMSTATE_PAUSED;

    return true;
}

bool ModemManager::shutdown(int index)
{
    if (index < 0 || index >= m_modemThreads.size())
        return false;

    return m_modems[index]->shutdown();
}

void ModemManager::startAll()
{
    for (int i = 0; i < (int)m_modemStates.size(); i++)
        m_modemStates[i] = MODEMSTATE_RUNNING;
}

void ModemManager::stopAll()
{
    for (int i = 0; i < (int)m_modemStates.size(); i++)
        m_modemStates[i] = MODEMSTATE_PAUSED;
}

void ModemManager::shutdownAll()
{
    foreach (ModemWorker* worker, m_modems) {
        if (!worker->shutdown())
            qWarning() << "could not shutdown modem worker";
    }
}

void ModemManager::frequencySelected(double frequency)
{
    m_modems.first()->getModem()->setFrequency(frequency);
}
