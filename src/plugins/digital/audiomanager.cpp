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

#include "audiomanager.h"
#include "audio/audiodevicelist.h"

using namespace Digital::Internal;

AudioManager::AudioManager(QObject* parent)
    : QObject(parent),
      m_deviceList(0),
      m_deviceIn(0),
      m_deviceOut(0)
{
    // enumerate devices
    m_deviceList = new AudioDeviceList(this);
    m_deviceList->enumerate();
}

void AudioManager::selectInputDevice()
{
    m_deviceIn = m_deviceList->getInputDevices().first();
    m_deviceIn->setSampleRate(8000);
    m_deviceIn->setSampleSize(16);
    m_deviceIn->init();
    m_deviceIn->start();
    emit inDeviceReady(m_deviceIn);
}

#include "audio/audioproducer.h"

void AudioManager::selectOutputDevice()
{
    m_deviceOut = m_deviceList->getOutputDevices().first();
    m_deviceOut->setSampleRate(8000);
    m_deviceOut->setSampleSize(16);
    m_deviceOut->init();

    /*AudioProducer* prod1 = new AudioProducer(this, 1000, 2, 1);
    m_deviceOut->registerProducer(prod1);

    AudioProducer* prod2 = new AudioProducer(this, 1500, 2.5, 1.5);
    m_deviceOut->registerProducer(prod2);

    AudioProducer* prod3 = new AudioProducer(this, 2000, 3.34, 3);
    m_deviceOut->registerProducer(prod3);*/

    emit outDeviceReady(m_deviceOut);
}

AudioDeviceIn* AudioManager::getInputDevice()
{
    return m_deviceIn;
}

AudioDeviceOut* AudioManager::getOutputDevice()
{
    return m_deviceOut;
}
