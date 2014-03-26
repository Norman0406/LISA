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

#include "digitalwindow.h"
#include "audiomanager.h"
#include "spectrumwindow.h"
#include "messengerwindow.h"
#include "modems/modemmanager.h"

#include <QVBoxLayout>
#include <coreplugin/minisplitter.h>

using namespace Digital::Internal;

DigitalWindow::DigitalWindow(QWidget *parent)
   : QWidget(parent),
     m_audioManager(0),
     m_spectrum(0),
     m_messenger(0)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    m_spectrum = new SpectrumWindow(this);
    m_messenger = new MessengerWindow(this);

    connect(m_spectrum, &SpectrumWindow::frequencySelected, m_messenger, &MessengerWindow::frequencySelected);
    connect(m_messenger, &MessengerWindow::frequencyChanged, m_spectrum, &SpectrumWindow::frequencyChanged);
    connect(m_messenger, &MessengerWindow::bandwidthChanged, m_spectrum, &SpectrumWindow::bandwidthChanged);
    connect(m_messenger, &MessengerWindow::modemActive, m_spectrum, &SpectrumWindow::modemActive);

    Core::MiniSplitter* splitter = new Core::MiniSplitter(Qt::Vertical);
    splitter->insertWidget(0, m_spectrum);
    splitter->insertWidget(1, m_messenger);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    QSize splitterSize = splitter->size();
    QList<int> sizes;
    sizes << splitterSize.height() / 3;
    splitter->setSizes(sizes);

    layout->addWidget(splitter);

    m_audioManager = new AudioManager(this);
    connect(m_audioManager, &AudioManager::inDeviceReady, m_spectrum, &SpectrumWindow::start);
    connect(m_audioManager, &AudioManager::inDeviceReady, m_messenger, &MessengerWindow::inDeviceReady);
    connect(m_audioManager, &AudioManager::outDeviceReady, m_messenger, &MessengerWindow::outDeviceReady);

    m_audioManager->selectInputDevice();
    m_audioManager->selectOutputDevice();
}
