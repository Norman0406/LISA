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

#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QWidget>

namespace Digital {
namespace Internal {

class SpectrumWidget
        : public QWidget
{
public:
    SpectrumWidget();

public slots:
    void addSpectrum(const QVector<double>&, double, double);
    void bandwidthChanged(double);
    void frequencyChanged(double);
    void modemActive(bool);    // a modem has been selected, lock the frequency

private slots:
    void redraw();
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMWIDGET_H
