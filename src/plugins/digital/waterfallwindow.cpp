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

#include "waterfallwindow.h"
#include "audio/audiodevicein.h"
#include "signalprocessing/spectrum.h"
#include "waterfalltoolbar.h"
#include "waterfall.h"
#include "spectrograph.h"

#include "audio/audiodevicelist.h"

#include <QVBoxLayout>

using namespace Digital::Internal;

WaterfallWindow::WaterfallWindow(QWidget* parent)
    : QWidget(parent),
      m_inputDevice(0),
      m_toolBar(new WaterfallToolBar(this)),
      m_waterfall(new Waterfall(this)),
      m_spectrograph(0)//new Spectrograph(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // TODO: init toolbar signals / slots
    layout->addWidget(m_toolBar);

    // connect spectrum to widget
    m_spectrum = new Spectrum(4096, WT_BLACKMANHARRIS, this);
    connect(m_spectrum, &Spectrum::spectrumLog, m_waterfall, &SpectrumWidget::addSpectrumLog);
    //connect(m_spectrum, &Spectrum::spectrumLog, m_spectrograph, &Spectrograph::addSpectrumLog);
    //connect(m_spectrum, &Spectrum::spectrumMag, m_spectrograph, &Spectrograph::addSpectrumMag);
    m_spectrum->init();

    // init timer to update the widgets
    m_timerThread = new QThread(this);
    m_spectrumTimer = new QTimer(0);
    m_spectrumTimer->setTimerType(Qt::PreciseTimer);
    m_spectrumTimer->setInterval(50);   // time in ms
    m_spectrumTimer->moveToThread(m_timerThread);
    connect(m_spectrumTimer, &QTimer::timeout, m_spectrum, &Spectrum::compute, Qt::DirectConnection);
    connect(m_timerThread, SIGNAL(started()), m_spectrumTimer, SLOT(start()));

    // set block size such that each block has a duration corresponding with m_spectrumTimer

    // init waterfall
    m_waterfall->init(m_spectrum->getSpectrumSize());

    layout->addWidget(m_waterfall);
    //layout->addWidget(m_spectrograph);

    setMinimumHeight(100);
}

WaterfallWindow::~WaterfallWindow()
{
    m_timerThread->quit();
    m_timerThread->wait();
}

void WaterfallWindow::start(AudioDeviceIn* inputDevice)
{
    m_inputDevice = inputDevice;

    double frameLength = 1.0 / (m_spectrumTimer->interval() * 0.001);
    qint64 blockSize = inputDevice->getFormat().sampleRate() / frameLength;
    m_spectrum->setNumSamples(blockSize);

    m_inputDevice->registerConsumer(m_spectrum);

    //m_spectrumTimer->start();
    m_timerThread->start();
}

void WaterfallWindow::stop()
{
    //m_spectrumTimer->stop();
    m_timerThread->quit();
    m_inputDevice->unregisterConsumer(m_spectrum);
    m_inputDevice = 0;
}

void WaterfallWindow::reset()
{
    m_waterfall->reset();
}

Waterfall* WaterfallWindow::getWaterfall()
{
    return m_waterfall;
}
