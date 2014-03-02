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

#ifndef WATERFALL_H
#define WATERFALL_H

#include "spectrumwidget.h"

namespace Digital {
namespace Internal {

class Colormap;

class Waterfall
        : public SpectrumWidget
{
    Q_OBJECT

public:
    explicit Waterfall(QWidget* parent);
    ~Waterfall(void);

    void reset();

    void setRefLevel(int);
    void setAmpSpan(int);

    int getRefLevel() const;
    int getAmpSpan() const;

protected:
    void iInit();
    void iAddSpectrum(const QVector<double>&, bool);
    void sizeChanged(const QSize&);

    void beginDraw(QPainter&);
    void drawSpectrum(QPainter&, const QRect&);
    QRect drawFrequencies(QPainter&);
    void drawMarkers(QPainter&, qreal, const QColor&, const QColor&);

private:
    void drawFrequencies();
    int log2disp(double);

    int         m_refLevel;
    int         m_ampSpan;
    QPixmap     m_waterfall;
    QPixmap     m_frequencies;

    // TODO: use either of them
    Colormap*	m_colorMap;
    QColor		m_colorTable[256];
};

} // namespace Internal
} // namespace Digital

#endif // WATERFALL_H
