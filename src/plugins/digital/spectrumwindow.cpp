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

#include "spectrumwindow.h"
#include "audio/audiodevicein.h"
#include "signalprocessing/fftspectrum.h"
#include "spectrumtoolbar.h"
#include "spectrumwaterfall.h"
#include "spectrumgraph.h"

#include "audio/audiodevicelist.h"

#include <QVBoxLayout>

using namespace Digital::Internal;

SpectrumWindow::SpectrumWindow(QWidget* parent)
    : QWidget(parent),
      m_inputDevice(0),
      m_toolBar(new SpectrumToolBar(this)),
      m_waterfall(new SpectrumWaterfall(this)),
      m_waterfallCombined(new SpectrumWaterfall(this)),
      m_graph(new SpectrumGraph(this)),
      m_graphCombined(new SpectrumGraph(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // connect spectrum to widget
    m_fftSpectrum = new FFTSpectrum(4096, WT_BLACKMANHARRIS, this);
    connect(m_fftSpectrum, &FFTSpectrum::spectrumLog, m_waterfall, &SpectrumWidget::addSpectrumLog);
    connect(m_fftSpectrum, &FFTSpectrum::spectrumLog, m_waterfallCombined, &SpectrumWidget::addSpectrumLog);
    connect(m_fftSpectrum, &FFTSpectrum::spectrumLog, m_graph, &SpectrumGraph::addSpectrumLog);
    connect(m_fftSpectrum, &FFTSpectrum::spectrumLog, m_graphCombined, &SpectrumGraph::addSpectrumLog);
    m_fftSpectrum->init();

    // create a threaded timer to update the widgets
    m_timerThread = new QThread(this);
    m_spectrumTimer = new QTimer(0);
    m_spectrumTimer->setTimerType(Qt::PreciseTimer);
    m_spectrumTimer->setInterval(50);   // time in ms
    m_spectrumTimer->moveToThread(m_timerThread);
    connect(m_spectrumTimer, &QTimer::timeout, m_fftSpectrum, &FFTSpectrum::compute, Qt::DirectConnection);
    connect(m_timerThread, SIGNAL(started()), m_spectrumTimer, SLOT(start()));

    // connect spectrum widgets
    connect(m_waterfall, &SpectrumWaterfall::frequencySelected, this, &SpectrumWindow::frequencySelected);
    connect(m_waterfallCombined, &SpectrumWaterfall::frequencySelected, this, &SpectrumWindow::frequencySelected);
    connect(m_waterfallCombined, &SpectrumWaterfall::mouseMoved, m_graphCombined, &SpectrumGraph::moveMouse);
    connect(m_waterfallCombined, &SpectrumWaterfall::mouseVisible, m_graphCombined, &SpectrumGraph::showMouse);
    connect(m_graph, &SpectrumGraph::frequencySelected, this, &SpectrumWindow::frequencySelected);
    connect(m_graphCombined, &SpectrumGraph::frequencySelected, this, &SpectrumWindow::frequencySelected);
    connect(m_graphCombined, &SpectrumGraph::mouseMoved, m_waterfallCombined, &SpectrumWaterfall::moveMouse);
    connect(m_graphCombined, &SpectrumGraph::mouseVisible, m_waterfallCombined, &SpectrumWaterfall::showMouse);

    // set block size such that each block has a duration corresponding with m_spectrumTimer

    // init spectrum displays
    m_waterfall->init(m_fftSpectrum->getSpectrumSize());
    m_waterfallCombined->init(m_fftSpectrum->getSpectrumSize());
    m_graph->init(m_fftSpectrum->getSpectrumSize());
    m_graphCombined->init(m_fftSpectrum->getSpectrumSize());

    // TODO: init toolbar signals / slots
    layout->addWidget(m_toolBar);
    connect(m_toolBar, &SpectrumToolBar::widgetSelected, this, &SpectrumWindow::widgetSelected);

    m_widget = new QStackedWidget(this);
    m_widget->addWidget(m_waterfall);
    m_toolBar->addSpectrumWidget(tr("Waterfall"));
    m_widget->addWidget(m_graph);
    m_toolBar->addSpectrumWidget(tr("Spectrum"));

    QWidget* combined = new QWidget(this);
    combined->setLayout(new QVBoxLayout(combined));
    combined->layout()->setSpacing(0);
    combined->layout()->setMargin(0);
    combined->layout()->addWidget(m_waterfallCombined);
    combined->layout()->addWidget(m_graphCombined);
    m_widget->addWidget(combined);
    m_toolBar->addSpectrumWidget(tr("Combined"));

    layout->addWidget(m_widget);

    setMinimumHeight(100);
}

SpectrumWindow::~SpectrumWindow()
{
    m_timerThread->quit();
    m_timerThread->wait();
}

void SpectrumWindow::start(AudioDeviceIn* inputDevice)
{
    m_inputDevice = inputDevice;

    double frameLength = 1.0 / (m_spectrumTimer->interval() * 0.001);
    qint64 blockSize = inputDevice->getFormat().sampleRate() / frameLength;
    m_fftSpectrum->setNumSamples(blockSize);

    m_inputDevice->registerConsumer(m_fftSpectrum);

    m_timerThread->start();
}

void SpectrumWindow::stop()
{
    m_timerThread->quit();
    m_inputDevice->unregisterConsumer(m_fftSpectrum);
    m_inputDevice = 0;
}

void SpectrumWindow::bandwidthChanged(double bandwidth)
{
    m_waterfall->bandwidthChanged(bandwidth);
    m_waterfallCombined->bandwidthChanged(bandwidth);
    m_graph->bandwidthChanged(bandwidth);
    m_graphCombined->bandwidthChanged(bandwidth);
}

void SpectrumWindow::frequencyChanged(double frequency)
{
    m_waterfall->frequencyChanged(frequency);
    m_waterfallCombined->frequencyChanged(frequency);
    m_graph->frequencyChanged(frequency);
    m_graphCombined->frequencyChanged(frequency);
}

void SpectrumWindow::modemActive(bool active)
{
    m_waterfall->modemActive(active);
    m_waterfallCombined->modemActive(active);
    m_graph->modemActive(active);
    m_graphCombined->modemActive(active);
}

void SpectrumWindow::reset()
{
    m_waterfall->reset();
    m_waterfallCombined->reset();
    m_graph->reset();
    m_graphCombined->reset();
}

void SpectrumWindow::widgetSelected(int index)
{
    m_widget->setCurrentIndex(index);

    // TODO: copy data from between waterfall widgets, such that each widget displays
    // the same waterfall without the need to work in background while it is not visible

    // or: find a more efficient way to work in background without a vector containing the
    // most recent data that can grow to infinity
}
