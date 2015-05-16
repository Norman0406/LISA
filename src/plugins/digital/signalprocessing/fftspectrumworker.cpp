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

#include "fftspectrumworker.h"
#include "fftspectrum.h"
#include "../audio/audiodevice.h"
#include <math.h>
#include <fftw3.h>

#include <QtMath>
#include <QtNumeric>

#include <QDebug>
#include <QThread>

using namespace Digital::Internal;

FFTSpectrumWorker::FFTSpectrumWorker(int fftSize, FFTWindow windowFunc)
    : m_sampleRate(-1),
      m_window(0),
      m_terminate(false),
      m_windowAvg(0)
{
    m_fftSize = fftSize;
    m_specSize = (m_fftSize / 2) + 1;
    m_binSize = 0;
    m_maxFrq = 0;

    // create the window function
    m_windowFunc = windowFunc;
    m_window = new double[m_fftSize];
    createWindow();

    m_spectrum.resize(m_specSize);
    m_spectrumMag.resize(m_specSize);
    m_spectrumLog.resize(m_specSize);
    m_spectrumPhase.resize(m_specSize);

    qDebug() << "using FFT size: " << m_fftSize;
}

FFTSpectrumWorker::~FFTSpectrumWorker()
{
    stop();
    delete[] m_window;
}

void FFTSpectrumWorker::stop()
{
    m_mutexIn.lock();
    m_terminate = true;
    m_dataReady.wakeAll();
    m_mutexIn.unlock();
}

void FFTSpectrumWorker::startFFT(const QAudioFormat& format, const QVector<double>& buffer)
{
    // set data
    m_mutexIn.lock();
    m_bufferIn = buffer;
    m_mutexIn.unlock();

    if (m_sampleRate != format.sampleRate()) {
        m_sampleRate = format.sampleRate();
        m_binSize = (m_sampleRate / 2.0) / m_specSize;
        m_maxFrq = m_sampleRate / 2.0;

        // resize output vectors if passband changed
        const int bins = m_specSize;
        if (m_spectrum.size() != bins)
            m_spectrum.resize(bins);
        if (m_spectrumMag.size() != bins)
            m_spectrumMag.resize(bins);
        if (m_spectrumLog.size() != bins)
            m_spectrumLog.resize(bins);
        if (m_spectrumPhase.size() != bins)
            m_spectrumPhase.resize(bins);
    }

    // wake thread to process next FFT
    m_dataReady.wakeAll();
}

void FFTSpectrumWorker::run()
{
    m_terminate = false;

    // create an fftw plan for real-2-complex fft
    double* in = (double*)fftw_malloc(sizeof(double) * m_fftSize);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_fftSize);
    fftw_plan my_plan = fftw_plan_dft_r2c_1d(m_fftSize, in, out, FFTW_ESTIMATE);

    while (!m_terminate) {
        // wait for incoming data to process
        m_mutexIn.lock();
        while (m_bufferIn.isEmpty() && !m_terminate)
            m_dataReady.wait(&m_mutexIn);
        m_mutexIn.unlock();

        if (!m_terminate) {

            //time.start();

            // latency means that only a portion of the input data is processed via fft.
            // a latency of 16 processes the whole input data of size m_fftSize while a
            // latency of 1 processes the first 1/16th of input data.
            double vscale = 2.0 / m_fftSize;
            int latency = 16;
            if (latency < 1)
                latency = 1;
            if (latency > 16)
                latency = 16;
            int nSamples = m_fftSize * latency / 16;
            vscale *= sqrt(16.0 / latency);

            // set input data
            m_mutexIn.lock();
            memset(in, 0, m_fftSize * sizeof(double));
            for (int i = 0; i < nSamples; i++)
                in[i] = m_window[i * 16 / latency] * m_bufferIn[i] * vscale;
            m_bufferIn.clear();
            m_mutexIn.unlock();

            // compute fft
            fftw_execute(my_plan);

            // lock to save output data
            m_mutexOut.lock();
            m_isOutputReady = false;

            // get spectrum
            for (int i = 0; i < m_specSize; i++) {
                std::complex<double> value(out[i][0], out[i][1]);

                double mag = getMagnitude(value);
                //double log = round(10.0 * log10(mag + 1e-10));
                double log = 10.0 * log10(mag);
                double phase = qAtan2(value.imag(), value.real());

                m_spectrum[i] = value;
                m_spectrumMag[i] = mag;
                m_spectrumLog[i] = log;
                m_spectrumPhase[i] = phase;
            }

            m_isOutputReady = true;
            m_mutexOut.unlock();
            m_outputReady.wakeAll();

            // signal that data is ready
            emit dataReady();
        }
    }

    fftw_destroy_plan(my_plan);
    fftw_free(in);
    fftw_free(out);

    // signal that thread is finished
    emit finished();
}

int FFTSpectrumWorker::getFFTSize() const
{
    return m_fftSize;
}

int FFTSpectrumWorker::getSpectrumSize() const
{
    return m_spectrum.size();
}

double FFTSpectrumWorker::getBinSize() const
{
    return m_binSize;
}

double FFTSpectrumWorker::getMaxFrq() const
{
    return m_maxFrq;
}

const QVector<std::complex<double> >& FFTSpectrumWorker::getSpectrum()
{
    // wait until data is ready
    m_mutexOut.lock();
    while (!m_isOutputReady)
        m_outputReady.wait(&m_mutexOut);
    m_mutexOut.unlock();

    return m_spectrum;
}

const QVector<double>& FFTSpectrumWorker::getSpectrumMag()
{
    // wait until data is ready
    m_mutexOut.lock();
    while (!m_isOutputReady)
        m_outputReady.wait(&m_mutexOut);
    m_mutexOut.unlock();

    return m_spectrumMag;
}

const QVector<double>& FFTSpectrumWorker::getSpectrumLog()
{
    // wait until data is ready
    m_mutexOut.lock();
    while (!m_isOutputReady)
        m_outputReady.wait(&m_mutexOut);
    m_mutexOut.unlock();

    return m_spectrumLog;
}

const QVector<double>& FFTSpectrumWorker::getSpectrumPhase()
{
    // wait until data is ready
    m_mutexOut.lock();
    while (!m_isOutputReady)
        m_outputReady.wait(&m_mutexOut);
    m_mutexOut.unlock();

    return m_spectrumPhase;
}

void FFTSpectrumWorker::createWindow()
{
    double pwr = 0;
    for (int i = 0; i < m_fftSize; i++) {
        m_window[i] = calcWindowFunc(i);
        pwr += m_window[i] * m_window[i];
    }

    m_windowAvg = 0;
    pwr = qSqrt((double)m_fftSize / pwr);
    for (int i = 0; i < m_fftSize; i++) {
        m_window[i] *= pwr;
        m_windowAvg += m_window[i];
    }
    m_windowAvg /= m_fftSize;
}

double FFTSpectrumWorker::calcWindowFunc(const int sample)
{
    const double val = sample / (double)m_fftSize;

    switch (m_windowFunc) {
    case WT_RECT:
        return 1.0;
    case WT_HANNING:
        return 0.5 - 0.5 * qCos(2 * M_PI * val);
    case WT_HAMMING:
        return 0.54 - 0.46 * cos(2 * M_PI * val);
    case WT_BLACKMAN:
        return 0.42 - 0.50 * qCos(2 * M_PI * val) + 0.08 * qCos(4 * M_PI * val);
    case WT_BLACKMANHARRIS:
        return 0.35875 - 0.48829 * qCos(2 * M_PI * val) + 0.14128 * qCos(4 * M_PI * val) - 0.01168 * qCos(6 * M_PI * val);
    default:
        return 0;
    }
}

double FFTSpectrumWorker::getMagnitude(const std::complex<double>& fftVal) const
{
    return std::norm(fftVal);
}

double FFTSpectrumWorker::getMagnitudeSqr(const std::complex<double>& fftVal) const
{
    return fftVal.real() * fftVal.real() + fftVal.imag() * fftVal.imag();
}
