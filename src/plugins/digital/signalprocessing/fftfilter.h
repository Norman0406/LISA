/*
 *    fftfilt.h  --  Fast convolution FIR filter
*/

#ifndef	FFTFILT_H
#define	FFTFILT_H

#include <complex>
#include <fftw3.h>
#include <qmath.h>

class FFTFilter {
public:
    FFTFilter(double f1, double f2, int len);
    FFTFilter(double f, int len);
    ~FFTFilter();

    // f1 < f2 ==> bandpass
    // f1 > f2 ==> band reject
    void createFilter(double f1, double f2);
    void createLPF(double f) {
        createFilter(0, f);
    }
    void createHPF(double f) {
        createFilter(f, 0);
    }
    void rttyFilter(double);

    int run(const std::complex<double>& in, std::complex<double>** out);

private:
    void initFilter();
    inline double fsinc(double fc, int i, int len) {
        return (i == len/2) ? 2.0 * fc:
                sin(2 * M_PI * fc * (i - len/2)) / (M_PI * (i - len/2));
    }
    inline double blackman(int i, int len) {
        return (0.42 -
                 0.50 * cos(2.0 * M_PI * i / len) +
                 0.08 * cos(4.0 * M_PI * i / len));
    }

    int m_flen;
    int m_flen2;

    fftw_complex*   m_fftIn;
    fftw_complex*   m_fftOut;
    fftw_plan       m_fftPlanForward;
    fftw_plan       m_fftPlanBackward;

    std::complex<double>* m_ht;
    std::complex<double>* m_filter;
    std::complex<double>* m_timedata;
    std::complex<double>* m_freqdata;
    std::complex<double>* m_ovlbuf;
    std::complex<double>* m_output;
    int m_inptr;
    int m_pass;
    int m_window;
};

#endif
