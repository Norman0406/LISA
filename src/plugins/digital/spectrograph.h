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

#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include "spectrumwidget.h"

namespace Digital {
namespace Internal {

class Spectrograph
        : public SpectrumWidget
{
public:
    explicit Spectrograph(QWidget* parent);
    ~Spectrograph(void);

    enum Mode
    {
        MODE_LOG,
        MODE_MAG
    };

    void setMode(Mode);

protected:
    void iInit();
    void iAddSpectrumMag(const QVector<double>&, bool);
    void sizeChanged(const QSize&);

    void beginDraw(QPainter&);
    void drawSpectrum(QPainter&, const QRect&);
    QRect drawFrequencies(QPainter&);
    void drawFrequencies();
    void drawMarkers(QPainter&, qreal, const QColor&, const QColor&);

private:
    //void drawSpectrum(QPainter&, const QVector<double>&);

    QVector<double> m_spectrumMag;
    Mode m_mode;
};

} // namespace Internal
} // namespace Digital

#endif // SPECTROGRAPH_H
