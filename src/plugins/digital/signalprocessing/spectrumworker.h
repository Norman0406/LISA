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

#ifndef SPECTRUMWORKER_H
#define SPECTRUMWORKER_H

#include <QObject>
#include <QAudioFormat>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>
#include <complex>
#include "Spectrum.h"

namespace Digital {
namespace Internal {

// threading method (not subclassing QThread) discussed in
// http://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
// http://blog.qt.digia.com/blog/2010/06/17/youre-doing-it-wrong/
class SpectrumWorker
        : public QObject
{
    Q_OBJECT

public:
    SpectrumWorker(int, SpectrumWindow);
    ~SpectrumWorker();

    void stop();

    int getFFTSize() const;
    int getSpectrumSize() const;
    double getBinSize() const;
    double getMaxFrq() const;
    const QVector<std::complex<double> >& getSpectrum();
    const QVector<double>& getSpectrumMag();
    const QVector<double>& getSpectrumLog();
    const QVector<double>& getSpectrumPhase();

public slots:
    void startFFT(const QAudioFormat&, const QVector<double>&);
    void run();

signals:
    void dataReady();
    void finished();

private:
    void createWindow();
    double calcWindowFunc(const int);
    double getMagnitude(const std::complex<double>&) const;
    double getMagnitudeSqr(const std::complex<double>&) const;

    int m_fftSize;
    int m_specSize;
    int m_sampleRate;
    double m_binSize;
    double m_maxFrq;
    SpectrumWindow m_windowFunc;
    double* m_window;
    bool m_terminate;
    double m_windowAvg;

    QMutex m_mutexIn;
    QWaitCondition m_dataReady;

    QMutex m_mutexOut;
    QWaitCondition m_outputReady;
    bool m_isOutputReady;

    QVector<double> m_bufferIn;

    QVector<std::complex<double> > m_spectrum;	// complex spectrum
    QVector<double> m_spectrumMag;      // magnitude spectrum
    QVector<double> m_spectrumLog;		// logarithmic spectrum
    QVector<double> m_spectrumPhase;	// spectrum's phase
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMWORKER_H
