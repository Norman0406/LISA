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
      m_waterfallCombined(new Waterfall(this)),
      m_spectrograph(new Spectrograph(this)),
      m_spectrographCombined(new Spectrograph(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // TODO: init toolbar signals / slots
    layout->addWidget(m_toolBar);
    connect(m_toolBar, &WaterfallToolBar::widgetSelected, this, &WaterfallWindow::widgetSelected);

    // connect spectrum to widget
    m_spectrum = new Spectrum(4096, WT_BLACKMANHARRIS, this);
    connect(m_spectrum, &Spectrum::spectrumLog, m_waterfall, &SpectrumWidget::addSpectrumLog);
    connect(m_spectrum, &Spectrum::spectrumLog, m_waterfallCombined, &SpectrumWidget::addSpectrumLog);
    connect(m_spectrum, &Spectrum::spectrumLog, m_spectrograph, &Spectrograph::addSpectrumLog);
    connect(m_spectrum, &Spectrum::spectrumMag, m_spectrograph, &Spectrograph::addSpectrumMag);
    connect(m_spectrum, &Spectrum::spectrumLog, m_spectrographCombined, &Spectrograph::addSpectrumLog);
    connect(m_spectrum, &Spectrum::spectrumMag, m_spectrographCombined, &Spectrograph::addSpectrumMag);
    m_spectrum->init();

    // init timer to update the widgets
    m_timerThread = new QThread(this);
    m_spectrumTimer = new QTimer(0);
    m_spectrumTimer->setTimerType(Qt::PreciseTimer);
    m_spectrumTimer->setInterval(50);   // time in ms
    m_spectrumTimer->moveToThread(m_timerThread);
    connect(m_spectrumTimer, &QTimer::timeout, m_spectrum, &Spectrum::compute, Qt::DirectConnection);
    connect(m_timerThread, SIGNAL(started()), m_spectrumTimer, SLOT(start()));

    // connect spectrum widgets
    connect(m_waterfall, &Waterfall::frequencySelected, this, &WaterfallWindow::frequencySelected);
    connect(m_waterfallCombined, &Waterfall::frequencySelected, this, &WaterfallWindow::frequencySelected);
    connect(m_waterfallCombined, &Waterfall::mouseMoved, m_spectrographCombined, &Spectrograph::moveMouse);
    connect(m_waterfallCombined, &Waterfall::mouseVisible, m_spectrographCombined, &Spectrograph::showMouse);
    connect(m_spectrograph, &Spectrograph::frequencySelected, this, &WaterfallWindow::frequencySelected);
    connect(m_spectrographCombined, &Spectrograph::frequencySelected, this, &WaterfallWindow::frequencySelected);
    connect(m_spectrographCombined, &Spectrograph::mouseMoved, m_waterfallCombined, &Waterfall::moveMouse);
    connect(m_spectrographCombined, &Spectrograph::mouseVisible, m_waterfallCombined, &Waterfall::showMouse);

    // set block size such that each block has a duration corresponding with m_spectrumTimer

    // init spectrum displays
    m_waterfall->init(m_spectrum->getSpectrumSize());
    m_waterfallCombined->init(m_spectrum->getSpectrumSize());
    m_spectrograph->init(m_spectrum->getSpectrumSize());
    m_spectrographCombined->init(m_spectrum->getSpectrumSize());

    m_widget = new QStackedWidget(this);
    m_widget->addWidget(m_waterfall);
    m_toolBar->addSpectrumWidget(tr("Waterfall"));
    m_widget->addWidget(m_spectrograph);
    m_toolBar->addSpectrumWidget(tr("Spectrum"));

    QWidget* combined = new QWidget(this);
    combined->setLayout(new QVBoxLayout(combined));
    combined->layout()->setSpacing(0);
    combined->layout()->setMargin(0);
    combined->layout()->addWidget(m_waterfallCombined);
    combined->layout()->addWidget(m_spectrographCombined);
    m_widget->addWidget(combined);
    m_toolBar->addSpectrumWidget(tr("Combined"));

    layout->addWidget(m_widget);

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

void WaterfallWindow::bandwidthChanged(double bandwidth)
{
    m_waterfall->bandwidthChanged(bandwidth);
    m_waterfallCombined->bandwidthChanged(bandwidth);
    m_spectrograph->bandwidthChanged(bandwidth);
    m_spectrographCombined->bandwidthChanged(bandwidth);
}

void WaterfallWindow::frequencyChanged(double frequency)
{
    m_waterfall->frequencyChanged(frequency);
    m_waterfallCombined->frequencyChanged(frequency);
    m_spectrograph->frequencyChanged(frequency);
    m_spectrographCombined->frequencyChanged(frequency);
}

void WaterfallWindow::modemActive(bool active)
{
    m_waterfall->modemActive(active);
    m_waterfallCombined->modemActive(active);
    m_spectrograph->modemActive(active);
    m_spectrographCombined->modemActive(active);
}

void WaterfallWindow::reset()
{
    m_waterfall->reset();
    m_waterfallCombined->reset();
    m_spectrograph->reset();
    m_spectrographCombined->reset();
}

void WaterfallWindow::widgetSelected(int index)
{
    m_widget->setCurrentIndex(index);

    // TODO: copy data from between waterfall widgets, such that each widget displays
    // the same waterfall without the need to work in background while it is not visible

    // or: find a more efficient way to work in background without a vector containing the
    // most recent data that can grow to infinity
}
