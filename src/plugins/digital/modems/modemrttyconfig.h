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

#ifndef MODEMRTTYCONFIG_H
#define MODEMRTTYCONFIG_H

#include "modemconfig.h"
#include "modemrtty.h"

namespace Digital {
namespace Internal {

class ModemRTTYConfig
        : public ModemConfig
{
    Q_OBJECT

public:
    ModemRTTYConfig(ModemRTTY*, QWidget*);

    void apply();

private:
    ModemRTTY*  m_modem;

    double      m_shift;
    int         m_bits;
    double      m_baud;
    ModemRTTY::Parity      m_parity;
    ModemRTTY::StopBits    m_stopBits;
    ModemRTTY::Demodulator m_demodulator;
    bool        m_unshiftOnSpace;
};

} // namespace Internal
} // namespace Digital

#endif // MODEMRTTYCONFIG_H
