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

#include "messengerwindow.h"
#include "messengertoolbar.h"
#include "messenger.h"

#include "modems/modemmanager.h"
#include "audio/audiodevicein.h"
#include "audio/audiodeviceout.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace Digital::Internal;

MessengerWindow::MessengerWindow(QWidget* parent)
    : QWidget(parent),
      m_toolBar(new MessengerToolBar(this)),
      m_messenger(new Messenger(this)),
      m_modemManager(new ModemManager(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    connect(m_toolBar, &MessengerToolBar::modemSelected, m_messenger, &Messenger::modemSelected);
    connect(m_toolBar, &MessengerToolBar::modemSelected, this, &MessengerWindow::modemSelected);
    connect(m_toolBar, &MessengerToolBar::clear, m_messenger, &Messenger::clear);

    layout->addWidget(m_toolBar);
    layout->addWidget(m_messenger);

    setMinimumHeight(100);

    connect(m_modemManager, &ModemManager::received, this, &MessengerWindow::received);
    connect(m_modemManager, &ModemManager::frequencyChanged, m_toolBar, &MessengerToolBar::frequencyChanged);
}

MessengerWindow::~MessengerWindow()
{
}

void MessengerWindow::inDeviceReady(AudioDeviceIn* inputDevice)
{
    inputDevice->registerConsumer(m_modemManager);
}

void MessengerWindow::outDeviceReady(AudioDeviceOut* outputDevice)
{
    //outputDevice->registerProducer(m_modemManager);
}

void MessengerWindow::received(int modemIndex, char character)
{
    // UNDONE: create multiple messagers for each modem and
    // route the decoded data accordingly

    Q_UNUSED(modemIndex);
    m_messenger->received(character);
}

void MessengerWindow::modemSelected(QString type)
{
    if (type.isEmpty()) {
        m_modemManager->terminateAll();
        emit modemActive(false);
    }
    else {
        m_modemManager->create(type);
        m_modemManager->startAll();
        emit modemActive(true);
    }
}

ModemManager* MessengerWindow::getModemManager()
{
    return m_modemManager;
}
