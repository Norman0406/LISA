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

#ifndef FFTSPECTRUM_H
#define FFTSPECTRUM_H

#include "../audio/audioconsumer.h"
#include "../audio/audioringbuffer.h"

#include <QObject>
#include <QAudioFormat>
#include <QVector>
#include <QTimer>
#include <QMutex>
#include <complex>
#include <QWaitCondition>

namespace Digital {
namespace Internal {

enum FFTWindow {
    WT_RECT = 0,
    WT_HANNING,	// or HANN?
    WT_HAMMING,
    WT_BLACKMAN,
    WT_BLACKMANHARRIS,
};

class FFTSpectrumWorker;

class FFTSpectrum
        : public AudioConsumer
{
    Q_OBJECT

public:
    FFTSpectrum(int, FFTWindow, QObject*);
    ~FFTSpectrum();

    void init();
    int getFFTSize() const;
    int getSpectrumSize() const;
    double getBinSize() const;
    double getMaxFrq() const;

public slots:
    void compute();

private slots:
    void spectrumReady();

signals:
    void spectrumLog(const QVector<double>&, double, double);
    void spectrumMag(const QVector<double>&, double, double);

protected:
    void registered();
    void unregistered();
    void audioDataReady(const QVector<double>& data);

private:
    int m_fftSize;
    QVector<double> m_spectrumLog;
    QVector<double> m_spectrumMag;
    FFTSpectrumWorker* m_fftWorker;
    QThread* m_fftThread;
    AudioRingBuffer* m_buffer;
};

} // namespace Internal
} // namespace Digital

#endif // FFTSPECTRUM_H
