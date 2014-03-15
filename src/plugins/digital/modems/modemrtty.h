/***********************************************************************
 *
 * LISA: Lightweight Integrated System for Amateur Radio
 * Copyright (C) 2013 - 2014
 *      Norman Link (DM6LN)
 *
 * This file is part of LISA. It contains the implementation for the
 * RTTY operating mode and has been adapted from the fldigi project
 * by Dave Freese, W1HKJ.
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

#ifndef MODEMRTTY_H
#define MODEMRTTY_H

#include "modem.h"
#include "../signalprocessing/fftfilter.h"
#include "../signalprocessing/filters.h"
#include <complex>

namespace Digital {
namespace Internal {

class Oscillator;
class SymbolShaper;

class ModemRTTY
        : public Modem
{
    Q_OBJECT

public:
    enum Parity {
        PARITY_NONE = 0,
        PARITY_EVEN,
        PARITY_ODD,
        PARITY_ZERO,
        PARITY_ONE
    };

    enum StopBits {
        STOP_1,     // 1
        STOP_15,    // 1.5
        STOP_2      // 2
    };

    enum Demodulator {
        DEMOD_LINEAR_ATC,
        DEMOD_CLIPPED_ATC,
        DEMOD_OPTIMAL_ATC,
        DEMOD_KAHN_LINEAR_ATC,
        DEMOD_KAHN_CLIPPED_ATC,
        DEMOD_NO_ATC,
    };

    ModemRTTY(QObject*);
    ~ModemRTTY();

    static QString getTypeStatic();
    QString getType() const;

    void setShift(double);
    void setBaud(double);
    void setBits(int);
    void setParity(Parity);
    void setStopBits(StopBits);
    void setDemodulator(Demodulator);
    void setUnshiftOnSpace(bool);

protected:
    bool iInit();
    void iRestart();
    void iShutdown();
    void iRxProcess(const QVector<double>&);
    void iTxProcess(ModemTransmitter*);
    double getBandwidth() const;

private:
    void    resetFilters();
    std::complex<double> mix(double& phase, double frq, std::complex<double> in);
    bool    rx(bool bit);
    int     rParity(int);
    int     rttyParity(unsigned int);
    bool    isMark();
    bool    isMarkSpace(int&);
    char    decode();
    int     baudotEnc(unsigned char data);
    char    baudotDec(unsigned char data);
    void    metric();

    void    sendSymbol(int symbol, int len, ModemTransmitter*);
    void    sendChar(int, ModemTransmitter*);
    void    sendStop(ModemTransmitter*);
    void    sendIdle(ModemTransmitter*);
    void    flushStream(ModemTransmitter*);

    // constants
    static const char   LETTERS[32];
    static const char   FIGURES[32];
    static const double SHIFTS[];
    static const double BAUDS[];
    static const int    BITS[];

    // general parameters
    int         m_symbolLen;
    double      m_shift;
    int         m_bits;
    double      m_baud;
    int         m_stopLen;
    Parity      m_parity;
    StopBits    m_stopBits;
    Demodulator m_demodulator;
    bool        m_unshiftOnSpace;

    // mark processing
    double      m_markPhase;
    double		m_markNoise;
    FFTFilter*  m_markFilter;
    double      m_markEnv;

    // space processing
    double      m_spacePhase;
    double		m_spaceNoise;
    FFTFilter*  m_spaceFilter;
    double      m_spaceEnv;

    std::complex<double> m_prevMark;
    std::complex<double> m_prevSpace;

    std::vector<bool> m_bitBuf;

    enum RxState {
        RXSTATE_IDLE = 0,
        RXSTATE_START,
        RXSTATE_DATA,
        RXSTATE_STOP
    } m_rxState;

    int m_counter;
    int m_bitCounter;
    int m_rxData;

    char m_lastChar;

    enum Mode {
        MODE_LETTERS = 0x100,
        MODE_FIGURES = 0x200
    } m_rxMode, m_txMode;

    Oscillator*     m_oscMark;
    Oscillator*     m_oscSpace;
    SymbolShaper*   m_symShaperMark;
    SymbolShaper*   m_symShaperSpace;

    bool            m_preamble;
    bool            m_stopFlag;
};

// simple oscillator-class
class Oscillator
{
public:
    Oscillator(double samplerate);
    ~Oscillator() {}

    double update(double frequency);

private:
    double m_phase;
    double m_samplerate;
};

class SymbolShaper
{
public:
    SymbolShaper(double baud = 45.45, double sr = 8000.0);
    ~SymbolShaper();

    void reset();
    void preset(double baud, double sr);
    void printSincTable();
    double update(bool state);

private:
    int     m_tableSize;
    double*	m_sincTable;

    bool	m_state;
    double	m_accumulator;
    long	m_counter0;
    long	m_counter1;
    long	m_counter2;
    long	m_counter3;
    long	m_counter4;
    long	m_counter5;
    double	m_factor0;
    double	m_factor1;
    double	m_factor2;
    double	m_factor3;
    double	m_factor4;
    double	m_factor5;
    double	m_baudRate;
    double	m_sampleRate;
};

} // namespace Internal
} // namespace Digital

#endif // MODEMRTTY_H
