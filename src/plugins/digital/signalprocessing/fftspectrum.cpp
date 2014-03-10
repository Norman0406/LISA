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

#include "fftspectrum.h"
#include "fftspectrumworker.h"
#include "../audio/audiodevice.h"

#include <QtMath>
#include <QtNumeric>
#include <QThread>
#include <QDebug>

using namespace Digital::Internal;

FFTSpectrum::FFTSpectrum(int fftSize, FFTWindow wdType, QObject* parent)
    : AudioConsumer(512, parent),
      m_fftSize(fftSize),
      m_fftWorker(0),
      m_fftThread(0),
      m_buffer(0)
{
    m_fftThread = new QThread(this);
    m_fftWorker = new FFTSpectrumWorker(fftSize, wdType);
    m_fftWorker->moveToThread(m_fftThread);

    connect(m_fftThread, &QThread::started, m_fftWorker, &FFTSpectrumWorker::run);
    connect(m_fftWorker, &FFTSpectrumWorker::finished, m_fftThread, &QThread::quit);
    connect(m_fftWorker, &FFTSpectrumWorker::finished, m_fftWorker, &FFTSpectrumWorker::deleteLater);
    connect(m_fftThread, &QThread::finished, m_fftThread, &QThread::deleteLater);
    connect(m_fftWorker, &FFTSpectrumWorker::dataReady, this, &FFTSpectrum::spectrumReady);
}

FFTSpectrum::~FFTSpectrum()
{
    m_fftWorker->stop();
    m_fftThread->quit();
    m_fftThread->wait();
}

void FFTSpectrum::init()
{
    // start processing thread
    if (m_fftThread)
        m_fftThread->start();
}

void FFTSpectrum::registered()
{
    m_buffer = new AudioRingBuffer(getFormat(), (qint64)m_fftWorker->getFFTSize(), this);
}

void FFTSpectrum::unregistered()
{
    delete m_buffer;
}

void FFTSpectrum::audioDataReady(const QVector<double>& data)
{
    if (m_buffer) {
        m_buffer->writeData(data);
    }
}

void FFTSpectrum::compute()
{
    if (m_fftWorker && m_buffer->getBufferLength() >= m_fftSize) {
        // TODO: avoid copying the buffer multiple times
        QVector<double> bufferData;
        m_buffer->getBuffer(bufferData);

        m_fftWorker->startFFT(getFormat(), bufferData);
    }
}

void FFTSpectrum::spectrumReady()
{
    m_spectrumLog = m_fftWorker->getSpectrumLog();
    m_spectrumMag = m_fftWorker->getSpectrumMag();

    double binSize = getBinSize();
    double maxFrq = getMaxFrq();

    emit spectrumLog(m_spectrumLog, binSize, maxFrq);
    emit spectrumMag(m_spectrumMag, binSize, maxFrq);
}

int FFTSpectrum::getFFTSize() const
{
    return m_fftWorker ? m_fftWorker->getFFTSize() : 0;
}

int FFTSpectrum::getSpectrumSize() const
{
    return m_fftWorker ? m_fftWorker->getSpectrumSize() : 0;
}

double FFTSpectrum::getBinSize() const
{
    return m_fftWorker ? m_fftWorker->getBinSize() : 0;
}

double FFTSpectrum::getMaxFrq() const
{
    return m_fftWorker ? m_fftWorker->getMaxFrq() : 0;
}
