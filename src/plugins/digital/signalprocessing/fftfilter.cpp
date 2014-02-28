// ----------------------------------------------------------------------------
//	fftfilt.cxx  --  Fast convolution Overlap-Add filter
//
// Filter implemented using overlap-add FFT convolution method
// h(t) characterized by Windowed-Sinc impulse response
//
// Reference:
//	 "The Scientist and Engineer's Guide to Digital Signal Processing"
//	 by Dr. Steven W. Smith, http://www.dspguide.com
//	 Chapters 16, 18 and 21
//
// Copyright (C) 2006-2008 Dave Freese, W1HKJ
//
// This file is part of fldigi.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <memory.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <typeinfo>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>

#include "misc.h"
#include "fftfilter.h"

//------------------------------------------------------------------------------
// fft filter
// f1 < f2 ==> band pass filter
// f1 > f2 ==> band reject filter
// f1 == 0 ==> low pass filter
// f2 == 0 ==> high pass filter
//------------------------------------------------------------------------------
FFTFilter::FFTFilter(double f1, double f2, int len)
{
    m_flen = len;
    initFilter();
    createFilter(f1, f2);
}

//------------------------------------------------------------------------------
// low pass filter
//------------------------------------------------------------------------------
FFTFilter::FFTFilter(double f, int len)
{
    m_flen = len;
    initFilter();
    createLPF(f);
}

FFTFilter::~FFTFilter()
{
    fftw_destroy_plan(m_fftPlanForward);
    fftw_destroy_plan(m_fftPlanBackward);
    fftw_free(m_fftIn);
    fftw_free(m_fftOut);

    delete[] m_filter;
    delete[] m_timedata;
    delete[] m_freqdata;
    delete[] m_output;
    delete[] m_ovlbuf;
    delete[] m_ht;
}

//------------------------------------------------------------------------------
// initialize the filter
// create forward and reverse FFTs
//------------------------------------------------------------------------------

void FFTFilter::initFilter()
{
    m_flen2 = m_flen >> 1;

    m_fftIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_flen);
    m_fftOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_flen);
    m_fftPlanForward = fftw_plan_dft_1d(m_flen, m_fftIn, m_fftOut, FFTW_FORWARD, FFTW_ESTIMATE);
    m_fftPlanBackward = fftw_plan_dft_1d(m_flen, m_fftIn, m_fftOut, FFTW_BACKWARD, FFTW_ESTIMATE);

    m_filter	= new std::complex<double>[m_flen];
    m_timedata	= new std::complex<double>[m_flen];
    m_freqdata	= new std::complex<double>[m_flen];
    m_output	= new std::complex<double>[m_flen];
    m_ovlbuf	= new std::complex<double>[m_flen2];
    m_ht		= new std::complex<double>[m_flen];

    memset(m_filter, 0, m_flen * sizeof(std::complex<double>));
    memset(m_timedata, 0, m_flen * sizeof(std::complex<double>));
    memset(m_freqdata, 0, m_flen * sizeof(std::complex<double>));
    memset(m_output, 0, m_flen * sizeof(std::complex<double>));
    memset(m_ovlbuf, 0, m_flen2 * sizeof(std::complex<double>));
    memset(m_ht, 0, m_flen * sizeof(std::complex<double>));

    m_inptr = 0;
}

void FFTFilter::createFilter(double f1, double f2)
{
    // initialize the filter to zero
    memset(m_ht, 0, m_flen * sizeof(std::complex<double>));

    // create the filter shape coefficients by fft
    // filter values initialized to the ht response h(t)
	bool b_lowpass, b_highpass;//, window;
	b_lowpass = (f2 != 0);
	b_highpass = (f1 != 0);

    for (int i = 0; i < m_flen2; i++) {
        m_ht[i] = 0;
        //combine lowpass / highpass
        // lowpass @ f2
        if (b_lowpass)
            m_ht[i] += fsinc(f2, i, m_flen2);
        // highighpass @ f1
        if (b_highpass)
            m_ht[i] -= fsinc(f1, i, m_flen2);
	}
    // highpass is delta[flen2/2] - h(t)
    if (b_highpass && f2 < f1)
        m_ht[m_flen2 / 2] += 1;

    for (int i = 0; i < m_flen2; i++)
        m_ht[i] *= blackman(i, m_flen2);

    // this may change since green fft is in place fft
    memcpy(m_fftIn, m_ht, m_flen * sizeof(std::complex<double>));

    // ht is flen complex points with imaginary all zero
    // first half describes h(t), second half all zeros
    // perform the cmplx forward fft to obtain H(w)
    // filter is flen/2 complex values

    fftw_execute(m_fftPlanForward);
    memcpy(m_filter, m_fftOut, m_flen * sizeof(std::complex<double>));

    // normalize the output filter for unity gain
	double scale = 0, mag;
    for (int i = 0; i < m_flen2; i++) {
        mag = abs(m_filter[i]);
        if (mag > scale)
            scale = mag;
	}
	if (scale != 0) {
        for (int i = 0; i < m_flen; i++)
            m_filter[i] /= scale;
	}

    // perform the reverse fft to obtain h(t)
    // for testing
    // uncomment to obtain filter characteristics
    /*
    cmplx *revht = new cmplx[flen];
    memcpy(revht, filter, flen * sizeof(cmplx));

    fft->InverseComplexFFT(revht);

    std::fstream fspec;
    fspec.open("fspec.csv", std::ios::out);
    fspec << "i,imp.re,imp.im,filt.re,filt.im,filt.abs,revimp.re,revimp.im\n";
    for (int i = 0; i < flen2; i++)
        fspec
            << i << "," << ht[i].real() << "," << ht[i].imag() << ","
            << filter[i].real() << "," << filter[i].imag() << ","
            << abs(filter[i]) << ","
            << revht[i].real() << "," << revht[i].imag() << ","
            << std::endl;
    fspec.close();
    delete [] revht;
    */
    m_pass = 2;
}

/*
 * Filter with fast convolution (overlap-add algorithm).
 */

int FFTFilter::run(const std::complex<double>& in, std::complex<double>** out)
{
    // collect flen/2 input samples
    m_timedata[m_inptr++] = in;

    if (m_inptr < m_flen2)
		return 0;

    if (m_pass)
        --m_pass; // filter output is not stable until 2 passes

    // FFT transpose to the frequency domain
    memcpy(m_fftIn, m_timedata, m_flen * sizeof(std::complex<double>));
    fftw_execute(m_fftPlanForward);

    for (int i = 0; i < m_flen; i++) {
        std::complex<double>* out = (std::complex<double>*)&m_fftOut[i];
        std::complex<double> res = *out * m_filter[i];
        m_fftIn[i][0] = res.real();
        m_fftIn[i][1] = res.imag();
    }

    // transform back to time domain
    fftw_execute(m_fftPlanBackward);
    memcpy(m_freqdata, m_fftOut, m_flen * sizeof(std::complex<double>));

    // overlap and add
    // save the second half for overlapping next inverse FFT
    for (int i = 0; i < m_flen2; i++) {
        m_output[i] = m_ovlbuf[i] + m_freqdata[i];
        m_ovlbuf[i] = m_freqdata[i + m_flen2];
	}

    // clear inbuf pointer
    m_inptr = 0;

    // signal the caller there is flen/2 samples ready
    if (m_pass)
        return 0;

    *out = m_output;
    return m_flen2;
}

//------------------------------------------------------------------------------
// rtty filter
//------------------------------------------------------------------------------

//bool print_filter = true; // flag to inhibit printing multiple copies

void FFTFilter::rttyFilter(double f)
{
    // Raised cosine filter designed iaw Section 1.2.6 of
    // Telecommunications Measurements, Analysis, and Instrumentation
    // by Dr. Kamilo Feher / Engineers of Hewlett-Packard
    //
    // Frequency scaling factor determined hueristically by testing various values
    // and measuring resulting decoder CER with input s/n = - 9 dB
    //
    //    K     CER
    //   1.0   .0244
    //   1.1   .0117
    //   1.2   .0081
    //   1.3   .0062
    //   1.4   .0054
    //   1.5   .0062
    //   1.6   .0076

	f *= 1.4;

	double dht;
    for(int i = 0; i < m_flen2; ++i) {
        double x = (double)i / (double)(m_flen2);

        // raised cosine response (changed for -1.0...+1.0 times Nyquist-f
        // instead of books versions ranging from -1..+1 times samplerate)

		dht =
			x <= 0 ? 1.0 :
			x > 2.0 * f ? 0.0 :
			cos((M_PI * x) / (f * 4.0));

		dht *= dht; // cos^2

        // amplitude equalized nyquist-channel response
		dht /= sinc(2.0 * i * f);

        m_filter[i] = std::complex<double>(dht * cos((double)i * -0.5 * M_PI),
                                           dht * sin((double)i * -0.5 * M_PI));

        m_filter[(m_flen-i) % m_flen] = std::complex<double>(dht * cos((double)i * +0.5 * M_PI),
                                                             dht * sin((double)i * +0.5 * M_PI));
	}

    // perform the reverse fft to obtain h(t)
    // for testing
    // uncomment to obtain filter characteristics
    /*
    cmplx *revht = new cmplx[flen];
    memcpy(revht, filter, flen * sizeof(cmplx));

    fft->InverseComplexFFT(revht);

    std::fstream fspec;
    fspec.open("rtty_filter.csv", std::ios::out);
    fspec << "i,filt.re,filt.im,filt.abs,,revimp.re,revimp.im\n";
    for (int i = 0; i < flen; i++)
        fspec
            << i << ","
            << filter[i].real() << "," << filter[i].imag() << "," << abs(filter[i])
            << ",," << revht[i].real() << "," << revht[i].imag()
            << std::endl;
    fspec.close();
    delete [] revht;
    */
    // start outputs after 2 full passes are complete
    m_pass = 2;
}

