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

#ifndef SPECTRUMWATERFALL_H
#define SPECTRUMWATERFALL_H

#include "spectrumwidget.h"

#include <QVector>

namespace Digital {
namespace Internal {

class Colormap;

class SpectrumWaterfall
        : public SpectrumWidget
{
    Q_OBJECT

public:
    explicit SpectrumWaterfall(QWidget* parent);
    ~SpectrumWaterfall(void);

    void reset();

protected:
    void iInit();
    void iAddSpectrumLog(const QVector<double>&, bool);
    void sizeChanged(const QSize&);
    void iRedraw();

    void paint(QPainter&);
    void beginDraw(QPainter&);
    void drawSpectrum(QPainter&, const QRect&);
    QRect drawFrequencies(QPainter&);
    void drawMarkers(QPainter&, qreal, const QColor&, const QColor&);

private:
    void drawFrequencies();
    int log2disp(double);

    QPixmap     m_frequencies;

    QImage      m_waterfall;
    QRgb*       m_scrollBuffer;
    int         m_scrollPosition;

    // TODO: use either of them
    Colormap*	m_colorMap;
    QColor		m_colorTable[256];
    QVector<QVector<double> > m_recentData;
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMWATERFALL_H
