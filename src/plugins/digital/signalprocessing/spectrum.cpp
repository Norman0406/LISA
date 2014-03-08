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

#include "spectrum.h"
#include "spectrumworker.h"
#include "../audio/audiodevice.h"

#include <QtMath>
#include <QtNumeric>
#include <QThread>
#include <QDebug>

using namespace Digital::Internal;

Spectrum::Spectrum(int fftSize, SpectrumWindow wdType, QObject* parent)
    : AudioConsumer(512, parent),
      m_fftSize(fftSize),
      m_fftWorker(0),
      m_fftThread(0),
      m_buffer(0)
{
    m_fftThread = new QThread(this);
    m_fftWorker = new SpectrumWorker(fftSize, wdType);
    m_fftWorker->moveToThread(m_fftThread);

    connect(m_fftThread, &QThread::started, m_fftWorker, &SpectrumWorker::run);
    connect(m_fftWorker, &SpectrumWorker::finished, m_fftThread, &QThread::quit);
    connect(m_fftWorker, &SpectrumWorker::finished, m_fftWorker, &SpectrumWorker::deleteLater);
    connect(m_fftThread, &QThread::finished, m_fftThread, &QThread::deleteLater);
    connect(m_fftWorker, &SpectrumWorker::dataReady, this, &Spectrum::spectrumReady);
}

Spectrum::~Spectrum()
{
    m_fftWorker->stop();
    m_fftThread->quit();
    m_fftThread->wait();
}

void Spectrum::init()
{
    // start processing thread
    if (m_fftThread)
        m_fftThread->start();
}

void Spectrum::registered()
{
    m_buffer = new AudioRingBuffer(getFormat(), (qint64)m_fftWorker->getFFTSize(), this);
}

void Spectrum::unregistered()
{
    delete m_buffer;
}

void Spectrum::audioDataReady(const QVector<double>& data)
{
    if (m_buffer) {
        m_buffer->writeData(data);
    }
}

void Spectrum::compute()
{
    if (m_fftWorker && m_buffer->getBufferLength() >= m_fftSize) {
        // TODO: avoid copying the buffer multiple times
        QVector<double> bufferData;
        m_buffer->getBuffer(bufferData);

        m_fftWorker->startFFT(getFormat(), bufferData);
    }
}

void Spectrum::spectrumReady()
{
    m_spectrumLog = m_fftWorker->getSpectrumLog();
    m_spectrumMag = m_fftWorker->getSpectrumMag();

    double binSize = getBinSize();
    double maxFrq = getMaxFrq();

    emit spectrumLog(m_spectrumLog, binSize, maxFrq);
    emit spectrumMag(m_spectrumMag, binSize, maxFrq);
}

int Spectrum::getFFTSize() const
{
    return m_fftWorker ? m_fftWorker->getFFTSize() : 0;
}

int Spectrum::getSpectrumSize() const
{
    return m_fftWorker ? m_fftWorker->getSpectrumSize() : 0;
}

double Spectrum::getBinSize() const
{
    return m_fftWorker ? m_fftWorker->getBinSize() : 0;
}

double Spectrum::getMaxFrq() const
{
    return m_fftWorker ? m_fftWorker->getMaxFrq() : 0;
}
