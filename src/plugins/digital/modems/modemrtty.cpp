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

#include "modemrtty.h"
#include "../signalprocessing/misc.h"
#include <math.h>
#include <QDebug>

using namespace Digital::Internal;

const char ModemRTTY::LETTERS[32] = {
    '\0',	'E',	'\n',	'A',	' ',	'S',	'I',	'U',
    '\r',	'D',	'R',	'J',	'N',	'F',	'C',	'K',
    'T',	'Z',	'L',	'W',	'H',	'Y',	'P',	'Q',
    'O',	'B',	'G',	' ',	'M',	'X',	'V',	' '
};
const char ModemRTTY::FIGURES[32] = {
    '\0',	'3',	'\n',	'-',	' ',	'\a',	'8',	'7',
    '\r',	'$',	'4',	'\'',	',',	'!',	':',	'(',
    '5',	'"',	')',	'2',	'#',	'6',	'0',	'1',
    '9',	'?',	'&',	' ',	'.',	'/',	';',	' '
};
const double ModemRTTY::SHIFTS[] = {23, 85, 160, 170, 182, 200, 240, 350, 425, 850};
const double ModemRTTY::BAUDS[]  = {45, 45.45, 50, 56, 75, 100, 110, 150, 200, 300};
const int    ModemRTTY::BITS[]  = {5, 7, 8};

ModemRTTY::ModemRTTY(QObject* parent)
    : Modem(CAP_TX | CAP_RX | CAP_AFC | CAP_REV, parent),
      m_markFilter(0),
      m_spaceFilter(0)
{
}

ModemRTTY::~ModemRTTY()
{
    delete m_markFilter;
    delete m_spaceFilter;
    delete m_symShaperMark;
    delete m_symShaperSpace;
    delete m_oscMark;
    delete m_oscSpace;
}

QString ModemRTTY::getTypeStatic()
{
    return QString::fromAscii("RTTY");
}

QString ModemRTTY::getType() const
{
    return ModemRTTY::getTypeStatic();
}

void ModemRTTY::setShift(double shift)
{
    m_shift = shift;
    emit bandwidthChanged(m_shift);
}

void ModemRTTY::setBaud(double baud)
{
    m_baud = baud;

    if (getState() != STATE_PREINIT)
        restart();
}

void ModemRTTY::setBits(int bits)
{
    bool found = false;
    int len = sizeof(BITS) / sizeof(int);
    for (int i = 0; i < len; i++)
        if (bits == BITS[i])
            found = true;

    if (found) {
        m_bits = bits;

        if (getState() != STATE_PREINIT)
            restart();
    }
    else
        qWarning() << "invalid bits: " << bits;
}

void ModemRTTY::setParity(Parity parity)
{
    if (m_bits == 5)
        m_parity = PARITY_NONE;
    else
        m_parity = parity;
}

void ModemRTTY::setStopBits(StopBits stop)
{
    m_stopBits = stop;

    if (getState() != STATE_PREINIT)
        restart();
}

void ModemRTTY::setDemodulator(Demodulator demodulator)
{
    m_demodulator = demodulator;
}

void ModemRTTY::setUnshiftOnSpace(bool unshiftOnSpace)
{
    m_unshiftOnSpace = unshiftOnSpace;
}

bool ModemRTTY::iInit()
{
    m_symShaperMark = new SymbolShaper(45, getSampleRate());    // what about m_baud?
    m_symShaperSpace = new SymbolShaper(45, getSampleRate());

    m_oscMark = new Oscillator(getSampleRate());
    m_oscSpace = new Oscillator(getSampleRate());

    // set defaults
    setShift(SHIFTS[3]);
    setBaud(BAUDS[1]);
    setBits(BITS[0]);
    setParity(PARITY_NONE);
    setStopBits(STOP_15);
    setDemodulator(DEMOD_OPTIMAL_ATC);
    setUnshiftOnSpace(false);

    m_rxState = RXSTATE_IDLE;

    m_preamble = true;

    return true;
}

double ModemRTTY::getBandwidth() const
{
    return m_shift;
}

void ModemRTTY::iRestart()
{
    m_rxMode = MODE_LETTERS;
    m_txMode = MODE_LETTERS;

    m_symbolLen = (int)(getSampleRate() / m_baud + 0.5);
    m_bitBuf.resize(m_symbolLen);
    m_bitBuf.assign(m_bitBuf.size(), false);

    resetFilters();

    double stop = 2.0;
    if (m_stopBits == STOP_1)
        stop = 1.0;
    else if (m_stopBits == STOP_15)
        stop = 1.5;
    else
        stop = 2.0;

    m_stopLen = (int)(stop * getSampleRate() / m_baud + 0.5);

    m_markPhase = m_spacePhase = 0;
    m_markNoise = m_spaceNoise = 0;
    m_markEnv = m_spaceEnv = 0;
    m_prevMark = m_prevSpace = std::complex<double>(0, 0);

    m_lastChar = 0;
}

void ModemRTTY::iRxProcess(const QVector<double>& buffer)
{
    if (buffer.isEmpty())
        return;

    std::complex<double> z, zmark, zspace, *zp_mark, *zp_space;

    metric();

    for (int i = 0; i < buffer.size(); i++) {
        // Create analytic signal from sound card input samples
        z = std::complex<double>(buffer[i], buffer[i]);

        // Mix it with the audio carrier frequency to create two baseband signals
        // mark and space that are separated and processed independently.
        // lowpass Windowed Sinc - Overlap-Add convolution filters.
        // The two fftfilt's are the same size and processed in sync
        // therefore the mark and space filters will concurrently have the
        // same size outputs available for further processing

        double markFrq = getFrequency() + m_shift / 2.0;
        zmark = mix(m_markPhase, markFrq, z);
        m_markFilter->run(zmark, &zp_mark);

        double spaceFrq = getFrequency() - m_shift / 2.0;
        zspace = mix(m_spacePhase, spaceFrq, z);
        int n_out = m_spaceFilter->run(zspace, &zp_space);

        for (int j = 0; j < n_out; j++) {
            double markMag = abs(zp_mark[j]);
            m_markEnv = decayAvg(m_markEnv, markMag,
                                 (markMag > m_markEnv) ? m_symbolLen / 4 : m_symbolLen * 16);
            m_markNoise = decayAvg(m_markNoise, markMag,
                                   (markMag < m_markNoise) ? m_symbolLen / 4 : m_symbolLen * 48);

            double spaceMag = abs(zp_space[j]);
            m_spaceEnv = decayAvg(m_spaceEnv, spaceMag,
                                  (spaceMag > m_spaceEnv) ? m_symbolLen / 4 : m_symbolLen * 16);
            m_spaceNoise = decayAvg(m_spaceNoise, spaceMag,
                                    (spaceMag < m_spaceNoise) ? m_symbolLen / 4 : m_symbolLen * 48);

            // which one is better?
            //double noiseFloor = std::min(m_spaceNoise, m_markNoise);  // found in fldigi
            double noiseFloor = (m_spaceNoise + m_markNoise) / 2.0;     // described in the website below

            const int cwi = 0;   // for debug purposes only
            switch (cwi) {
                case 1 : // mark only decode
                    m_spaceEnv = noiseFloor;
                    break;
                case 2: // space only decode
                    m_markEnv = noiseFloor;
                default : ;
            }

            // demodulators are described here: http://www.w7ay.net/site/Technical/ATC/
            double value = 0;
            switch (m_demodulator) {
            case DEMOD_LINEAR_ATC:
                value = markMag - spaceMag - 0.5 * (m_markEnv - m_spaceEnv);
                break;
            case DEMOD_CLIPPED_ATC:
                markMag = markMag > m_markEnv ? m_markEnv : (markMag < noiseFloor ? noiseFloor : markMag);
                spaceMag = spaceMag > m_spaceEnv ? m_spaceEnv : (spaceMag < noiseFloor ? noiseFloor : spaceMag);
                value = (markMag - noiseFloor) - (spaceMag - noiseFloor) - 0.5 * (
                        (m_markEnv - noiseFloor) - (m_spaceEnv - noiseFloor));
                break;
            case DEMOD_OPTIMAL_ATC:
                markMag = markMag > m_markEnv ? m_markEnv : (markMag < noiseFloor ? noiseFloor : markMag);
                spaceMag = spaceMag > m_spaceEnv ? m_spaceEnv : (spaceMag < noiseFloor ? noiseFloor : spaceMag);
                value = (markMag - noiseFloor) * (m_markEnv - noiseFloor) -
                        (spaceMag - noiseFloor) * (m_spaceEnv - noiseFloor) - 0.5 * (
                        (m_markEnv - noiseFloor) * (m_markEnv - noiseFloor) -
                        (m_spaceEnv - noiseFloor) * (m_spaceEnv - noiseFloor));
                break;
            case DEMOD_KAHN_LINEAR_ATC:
                value = (markMag - noiseFloor) * (markMag - noiseFloor) -
                        (spaceMag - noiseFloor) * (spaceMag - noiseFloor) - 0.25 * (
                        (m_markEnv - noiseFloor) * (m_markEnv - noiseFloor) -
                        (m_spaceEnv - noiseFloor) * (m_spaceEnv - noiseFloor));
                break;
            case DEMOD_KAHN_CLIPPED_ATC:
                markMag = markMag > m_markEnv ? m_markEnv : (markMag < noiseFloor ? noiseFloor : markMag);
                spaceMag = spaceMag > m_spaceEnv ? m_spaceEnv : (spaceMag < noiseFloor ? noiseFloor : spaceMag);
                value = (markMag - noiseFloor) * (markMag - noiseFloor) -
                        (spaceMag - noiseFloor) * (spaceMag - noiseFloor) - 0.25 * (
                        (m_markEnv - noiseFloor) * (m_markEnv - noiseFloor) -
                        (m_spaceEnv - noiseFloor) * (m_spaceEnv - noiseFloor));
                break;
            case DEMOD_NO_ATC: // No ATC
            default :
                value = markMag - spaceMag;
            }

            bool bit = value > 0;

            // detect TTY signal transitions
            // rx(...) returns true if valid TTY bit stream detected
            // either character or idle signal
            bool reverse = getReverse();
            if (rx(reverse ? !bit : bit)) {
                double frqErr = (TWO_PI * getSampleRate() / m_baud) *
                        (!reverse ?
                            std::arg(std::conj(zp_mark[j]) * m_prevMark) :
                            std::arg(std::conj(zp_space[j]) * m_prevSpace));

                if (fabs(frqErr) > m_baud / 2)
                    frqErr = 0;
                else
                    adjustFrequency(frqErr);
            }

            m_prevMark = zp_mark[j];
            m_prevSpace = zp_space[j];
        }
    };
}

void ModemRTTY::iTxProcess(ModemTransmitter* transmitter)
{
    int c;

    if (m_preamble) {
        m_symShaperMark->reset();
        m_symShaperSpace->reset();

        for (int i = 0; i < m_bits + 1; i++)
            sendSymbol(0, m_symbolLen, transmitter);

        sendStop(transmitter);

        for (int i = 0; i < m_bits + 1; i++)
            sendSymbol(1, m_symbolLen, transmitter);

        sendStop(transmitter);
        sendIdle(transmitter);
        m_preamble = false;
        // freq1 = get_txfreq_woffset() + shift / 2.0;
    }
    // TODO: get char to send
    //c = get_tx_char();

    // TX buffer empty
    //if (c == GET_TX_CHAR_ETX || m_stopFlag) {
    if (false) {
        m_stopFlag = false;
        //line_char_count = 0;
        if (m_bits != 5) {
            /*if (progdefaults.rtty_crcrlf)
                send_char('\r');*/
            sendChar('\r', transmitter);
            sendChar('\n', transmitter);
        } else {
            /*if (progdefaults.rtty_crcrlf)
                send_char(0x08);*/
            sendChar(0x08, transmitter);
            sendChar(0x02, transmitter);
        }
        flushStream(transmitter);
        //cwid();
        //return -1;
        return;
    }

    // send idle character if c == -1
    //if (c == GET_TX_CHAR_NODATA) {
    if (false) {
        sendIdle(transmitter);
        return;
    }

    // if NOT Baudot
    if (m_bits != 5) {
        //acc_symbols = 0;
        sendChar(c, transmitter);
        //xmt_samples = char_samples = acc_symbols;
        //return 0;
        return;
    }

    /*if (isalpha(c) || isdigit(c) || isblank(c) || ispunct(c)) {
        ++line_char_count;
    }*/

    if (c == '\r') {
        //line_char_count = 0;
        sendChar(0x08, transmitter);
        //return 0;
        return;
    }

    bool crcCRLF = false;

    if (c == '\n') {
        //line_char_count = 0;
        if (crcCRLF)
            sendChar(0x08, transmitter); // CR-CR-LF triplet
        sendChar(0x02, transmitter);
        //return 0;
        return;
    }

    // unshift-on-space
    if (c == ' ') {
        if (m_unshiftOnSpace) {
            sendChar(MODE_LETTERS, transmitter);
            sendChar(0x04, transmitter); // coded value for a space
            m_txMode = MODE_LETTERS;
        } else
            sendChar(0x04, transmitter);
        //return 0;
        return;
    }

    if ((c = baudotEnc(c)) < 0) {
        //return 0;
        return;
    }

    // switch case if necessary

    if ((c & 0x300) != m_txMode) {
        if (m_txMode == MODE_FIGURES) {
            sendChar(MODE_LETTERS, transmitter);
            m_txMode = MODE_LETTERS;
        } else {
            sendChar(MODE_FIGURES, transmitter);
            m_txMode = MODE_FIGURES;
        }
    }

    //acc_symbols = 0;
    sendChar(c & 0x1F, transmitter);
    //xmt_samples = char_samples = acc_symbols;
}

void ModemRTTY::iShutdown()
{
}

void ModemRTTY::resetFilters()
{
    int filterLength = 1024; // UNDONE: determine filter length by 2 * block size?

    if (m_markFilter) {
        m_markFilter->rttyFilter(m_baud / getSampleRate());
    }
    else {
        m_markFilter = new FFTFilter(m_baud / getSampleRate(), filterLength);
        m_markFilter->rttyFilter(m_baud / getSampleRate());
    }

    if (m_spaceFilter) {
        m_spaceFilter->rttyFilter(m_baud / getSampleRate());
    }
    else {
        m_spaceFilter = new FFTFilter(m_baud / getSampleRate(), filterLength);
        m_spaceFilter->rttyFilter(m_baud / getSampleRate());
    }
}

std::complex<double> ModemRTTY::mix(double& phase, double frq, std::complex<double> in)
{
    std::complex<double> z = std::complex<double>(cos(phase), sin(phase)) * in;

    phase -= TWO_PI * frq / getSampleRate();
    if (phase < -TWO_PI)
        phase += TWO_PI;

    return z;
}

bool ModemRTTY::rx(bool bit)
{
    bool flag = false;
    int correction;

    for (int i = 1; i < m_symbolLen; i++)
        m_bitBuf[i-1] = m_bitBuf[i];
    m_bitBuf[m_symbolLen - 1] = bit;

    switch (m_rxState) {
    case RXSTATE_IDLE:
        if (isMarkSpace(correction)) {
            m_rxState = RXSTATE_START;
            m_counter = correction;
        }
        break;
    case RXSTATE_START:
        if (--m_counter == 0) {
            if (!isMark()) {
                m_rxState = RXSTATE_DATA;
                m_counter = m_symbolLen;
                m_bitCounter = 0;
                m_rxData = 0;
            } else {
                m_rxState = RXSTATE_IDLE;
            }
        }
        break;
    case RXSTATE_DATA:
        if (--m_counter == 0) {
            m_rxData |= isMark() << m_bitCounter++;
            m_counter = m_symbolLen;
        }
        if (m_bitCounter == m_bits + (m_parity != PARITY_NONE ? 1 : 0))
            m_rxState = RXSTATE_STOP;
        break;
    case RXSTATE_STOP:
        if (--m_counter == 0) {
            if (isMark()) {
                //if ((metric >= progStatus.sldrSquelchValue && progStatus.sqlonoff) || !progStatus.sqlonoff) {
                if (squelchOpen()) {
                    char c = decode();
                    if (c != 0) {
                        // supress <CR><CR> and <LF><LF> sequences
                        // these were observed during the RTTY contest 2/9/2013
                        if (c == '\r' && m_lastChar == '\r');
                        else if (c == '\n' && m_lastChar == '\n');
                        else {
                            emit received((char)c);
                        }
                        m_lastChar = c;
                    }
                    flag = true;
                }
            }
            m_rxState = RXSTATE_IDLE;
        }
        break;
    default:
        break;
    }

    return flag;
}

int ModemRTTY::rParity(int c)
{
    int w = c;
    int p = 0;
    while (w) {
        p += (w & 1);
        w >>= 1;
    }
    return p & 1;
}

int ModemRTTY::rttyParity(unsigned int c)
{
    c &= (1 << m_bits) - 1;

    switch (m_parity) {
    default:
    case PARITY_NONE:
        return 0;
    case PARITY_ODD:
        return rParity(c);
    case PARITY_EVEN:
        return !rParity(c);
    case PARITY_ZERO:
        return 0;
    case PARITY_ONE:
        return 1;
    }
}

bool ModemRTTY::isMark()
{
    return m_bitBuf[m_symbolLen / 2];
}

bool ModemRTTY::isMarkSpace(int& correction)
{
    correction = 0;
    // test for rough bit position
    if (m_bitBuf.front() && !m_bitBuf.back()) {
        // test for mark/space straddle point
        for (int i = 0; i < m_symbolLen; i++)
            correction += m_bitBuf[i];
        if (abs(m_symbolLen / 2 - correction) < 6) // too small & bad signals are not decoded
            return true;
    }
    return false;
}

char ModemRTTY::decode()
{
    unsigned int parbit, par, data;

    parbit = (m_rxData >> m_bits) & 1;
    par = rttyParity(m_rxData);

    if (m_parity != PARITY_NONE && parbit != par)
        return 0;

    data = m_rxData & ((1 << m_bits) - 1);

    if (m_bits == 5)
        return baudotDec(data);

    return data;
}

int ModemRTTY::baudotEnc(unsigned char data)
{
    int i, c, mode;

    mode = 0;
    c = -1;

    if (islower(data))
        data = toupper(data);

    for (i = 0; i < 32; i++) {
        if (data == LETTERS[i]) {
            mode |= MODE_LETTERS;
            c = i;
        }
        if (data == FIGURES[i]) {
            mode |= MODE_FIGURES;
            c = i;
        }
        if (c != -1)
            return (mode | c);
    }

    return -1;
}

char ModemRTTY::baudotDec(unsigned char data)
{
    int out = 0;

    switch (data) {
    case 0x1F:		/* letters */
        m_rxMode = MODE_LETTERS;
        break;
    case 0x1B:		/* figures */
        m_rxMode = MODE_FIGURES;
        break;
    case 0x04:		/* unshift-on-space */
        if (m_unshiftOnSpace)
            m_rxMode = MODE_LETTERS;
        return ' ';
        break;
    default:
        if (m_rxMode == MODE_LETTERS)
            out = LETTERS[data];
        else
            out = FIGURES[data];
        break;
    }

    return out;
}

void ModemRTTY::metric()
{
    /*double delta = rtty_baud/8.0;
    double np = wf->powerDensity(frequency, delta) * 3000 / delta;
    double sp =
        wf->powerDensity(frequency - shift/2, delta) +
        wf->powerDensity(frequency + shift/2, delta) + 1e-10;
    double snr = 0;

    sigpwr = decayavg( sigpwr, sp, sp > sigpwr ? 2 : 8);
    noisepwr = decayavg( noisepwr, np, 16 );
    snr = 10*log10(sigpwr / noisepwr);

    snprintf(snrmsg, sizeof(snrmsg), "s/n %-3.0f dB", snr);
    put_Status2(snrmsg);
    metric = CLAMP((3000 / delta) * (sigpwr/noisepwr), 0.0, 100.0);
    display_metric(metric);*/
}

void ModemRTTY::sendSymbol(int symbol, int len, ModemTransmitter* transmitter)
{
    //acc_symbols += len;
#if 0
    double freq;

    if (reverse)
        symbol = !symbol;

    if (symbol)
        freq = get_txfreq_woffset() + shift / 2.0;
    else
        freq = get_txfreq_woffset() - shift / 2.0;

    for (int i = 0; i < len; i++) {
        outbuf[i] = nco(freq);
        if (symbol)
            FSKbuf[i] = FSKnco();
        else
            FSKbuf[i] = 0.0 * FSKnco();
    }

#else

    double const freq1 = getFrequency() + m_shift / 2.0;
    double const freq2 = getFrequency() - m_shift / 2.0;
    double mark = 0, space = 0;

    if (getReverse())
        symbol = !symbol;

    for(int i = 0; i < len; ++i) {
        mark  = m_symShaperMark->update(symbol) * m_oscMark->update(freq1);
        space = m_symShaperSpace->update(!symbol) * m_oscSpace->update(freq2);
        transmitter->writeSample(mark + space);
        /*outbuf[i] = mark + space;

        if (minamp > outbuf[i])
            minamp = outbuf[i];
        if (maxamp < outbuf[i])
            maxamp = outbuf[i];*/
    }
#endif

    //ModulateXmtr(outbuf, symbollen);
}

void ModemRTTY::sendChar(int c, ModemTransmitter* transmitter)
{
    int i;

    if (m_bits == 5) {
        if (c == MODE_LETTERS)
            c = 0x1F;
        if (c == MODE_FIGURES)
            c = 0x1B;
    }

    // start bit
    sendSymbol(0, m_symbolLen, transmitter);
    // data bits
    for (i = 0; i < m_bits; i++) {
        sendSymbol((c >> i) & 1, m_symbolLen, transmitter);
    }
    // parity bit
    if (m_parity != PARITY_NONE)
        sendSymbol(rttyParity(c), m_symbolLen, transmitter);
    // stop bit(s)
    sendStop(transmitter);

    if (m_bits == 5) {
        if (c == 0x1F || c == 0x1B)
            return;
        if (m_txMode == MODE_LETTERS)
            c = LETTERS[c];
        else
            c = FIGURES[c];

        /*if (c)
            put_echo_char(progdefaults.rx_lowercase ? tolower(c) : c);*/
    }
    /*else if (c)
        put_echo_char(c);*/
}

void ModemRTTY::sendStop(ModemTransmitter* transmitter)
{
    //acc_symbols += len;
#if 0
    double freq;

    if (reverse)
        symbol = !symbol;

    if (symbol)
        freq = get_txfreq_woffset() + shift / 2.0;
    else
        freq = get_txfreq_woffset() - shift / 2.0;

    for (int i = 0; i < len; i++) {
        outbuf[i] = nco(freq);
        if (symbol)
            FSKbuf[i] = FSKnco();
        else
            FSKbuf[i] = 0.0 * FSKnco();
    }

#else

    double const freq1 = getFrequency() + m_shift / 2.0;
    double const freq2 = getFrequency() - m_shift / 2.0;
    double mark = 0, space = 0;

    bool symbol = true;

    if (getReverse())
        symbol = !symbol;

    for (int i = 0; i < m_stopLen; ++i) {
        mark  = m_symShaperMark->update(symbol) * m_oscMark->update(freq1);
        space = m_symShaperSpace->update(!symbol) * m_oscSpace->update(freq2);
        transmitter->writeSample(mark + space);
        //outbuf[i] = mark + space;
    }
#endif

    //ModulateXmtr(outbuf, symbollen);
}

void ModemRTTY::sendIdle(ModemTransmitter* transmitter)
{
    if (m_bits == 5) {
        sendChar(MODE_LETTERS, transmitter);
        m_txMode = MODE_LETTERS;
    }
    else
        sendChar(0, transmitter);
}

void ModemRTTY::flushStream(ModemTransmitter* transmitter)
{
    double const freq1 = getFrequency() + m_shift / 2.0;
    double const freq2 = getFrequency() - m_shift / 2.0;
    double mark = 0, space = 0;

    for(int i = 0; i < m_symbolLen * 6; ++i) {
        mark  = m_symShaperMark->update(0) * m_oscMark->update(freq1);
        space = m_symShaperSpace->update(0) * m_oscSpace->update(freq2);
        transmitter->writeSample(mark + space);
        //outbuf[i] = mark + space;
    }

    //ModulateXmtr(outbuf, symbollen * 6);
}

Oscillator::Oscillator(double samplerate)
{
    m_phase = 0;
    m_samplerate = samplerate;
    //std::cerr << "samplerate for Oscillator:"<<m_samplerate<<"\n";
}

double Oscillator::update(double frequency)
{
    m_phase += frequency / m_samplerate * TWO_PI;
    if (m_phase > M_PI)
        m_phase -= TWO_PI;
    return (sin(m_phase));
}

SymbolShaper::SymbolShaper(double baud, double sr)
{
    m_sincTable = 0;
    preset(baud, sr);
}

SymbolShaper::~SymbolShaper()
{
}

void SymbolShaper::reset()
{
    m_state = false;
    m_accumulator = 0.0;
    m_counter0 = 1024;
    m_counter1 = 1024;
    m_counter2 = 1024;
    m_counter3 = 1024;
    m_counter4 = 1024;
    m_counter5 = 1024;
    m_factor0 = 0.0;
    m_factor1 = 0.0;
    m_factor2 = 0.0;
    m_factor3 = 0.0;
    m_factor4 = 0.0;
    m_factor5 = 0.0;
}

void SymbolShaper::preset(double baud, double sr)
{
    double m_baudRate = baud;
    double m_sampleRate = sr;

    //LOG_INFO("Shaper::reset( %f, %f )",  baud_rate, sample_rate);

    // calculate new table-size for six integrators

    m_tableSize = m_sampleRate / m_baudRate * 5.49;
    //LOG_INFO("Shaper::m_table_size = %d", m_table_size);

    // kill old sinc-table and get memory for the new one

    delete[] m_sincTable;
    m_sincTable = new double[m_tableSize];

    // set up the new sinc-table based on the new parameters

    long double sum = 0.0;

    for (int x = 0; x < m_tableSize; ++x) {
        int const offset = m_tableSize / 2;
        double const T = m_sampleRate / (m_baudRate * 2.0); // symbol-length in samples
        double const t = (x - offset); // symbol-time relative to zero

        m_sincTable[x] = rcos(t, T, 1.0);

        // calculate integral
        sum += m_sincTable[x];
    }

    // scale the values in the table so that the integral over it is as
    // exactly 1.0000000 as we can do this...

    for(int x = 0; x < m_tableSize; ++x) {
        m_sincTable[x] *= 1.0 / sum;
    }

    // reset internal states
    reset();
}

double SymbolShaper::update(bool state)
{
    if(m_state != state) {
        m_state = state;
        if( m_counter0 >= m_tableSize) {
            m_counter0 = 0;
            m_factor0 = state ? +1.0 : -1.0;
        } else if( m_counter1 >= m_tableSize) {
            m_counter1 = 0;
            m_factor1 = state ? +1.0 : -1.0;
        } else if( m_counter2 >= m_tableSize) {
            m_counter2 = 0;
            m_factor2 = state ? +1.0 : -1.0;
        } else if( m_counter3 >= m_tableSize) {
            m_counter3 = 0;
            m_factor3 = state ? +1.0 : -1.0;
        } else if( m_counter4 >= m_tableSize) {
            m_counter4 = 0;
            m_factor4 = state ? +1.0 : -1.0;
        } else if( m_counter5 >= m_tableSize) {
            m_counter5 = 0;
            m_factor5 = state ? +1.0 : -1.0;
        }
    }

    if(m_counter0 < m_tableSize)
        m_accumulator += m_factor0 * m_sincTable[m_counter0++];

    if(m_counter1 < m_tableSize)
        m_accumulator += m_factor1 * m_sincTable[m_counter1++];

    if(m_counter2 < m_tableSize)
        m_accumulator += m_factor2 * m_sincTable[m_counter2++];

    if(m_counter3 < m_tableSize)
        m_accumulator += m_factor3 * m_sincTable[m_counter3++];

    if(m_counter4 < m_tableSize)
        m_accumulator += m_factor4 * m_sincTable[m_counter4++];

    if(m_counter5 < m_tableSize)
        m_accumulator += m_factor5 * m_sincTable[m_counter5++];

    return (m_accumulator / sqrt(2));
}

void SymbolShaper::printSincTable()
{
    //for (int i = 0; i < 1024; i++) printf("%f\n", m_SincTable[i]);
}
