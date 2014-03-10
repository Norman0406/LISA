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

#ifndef SPECTRUMGRAPH_H
#define SPECTRUMGRAPH_H

#include "spectrumwidget.h"

namespace Digital {
namespace Internal {

class SpectrumGraph
        : public SpectrumWidget
{
public:
    explicit SpectrumGraph(QWidget* parent);
    ~SpectrumGraph(void);

protected:
    void iInit();
    void iAddSpectrumLog(const QVector<double>&, bool);
    void sizeChanged(const QSize&);
    void iRedraw();

    void beginDraw(QPainter&);
    void drawSpectrum(QPainter&, const QRect&);
    QRect drawFrequencies(QPainter&);
    void drawFrequencies();
    void drawMarkers(QPainter&, qreal, const QColor&, const QColor&);

private:
    static void bresenham(QImage&, QPoint, QPoint, const QColor&, const QColor&);
    double valueToDisp(double) const;
    QVector<double> m_spectrum;
    QPixmap m_background;
    QImage m_spectrumGraph;
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMGRAPH_H
