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

#include "spectrograph.h"

#include <QPainter>
#include <QtNumeric>
#include <QTimer>

using namespace Digital::Internal;

Spectrograph::Spectrograph(QWidget* parent)
    : QWidget(parent),
      m_mode(MODE_MAG)
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setTimerType(Qt::PreciseTimer);
    m_updateTimer->setInterval(30);   // time in ms
    connect(m_updateTimer, &QTimer::timeout, this, &Spectrograph::redraw);
    m_updateTimer->start();
}

Spectrograph::~Spectrograph(void)
{

}

void Spectrograph::setMode(Mode mode)
{
    m_mode = mode;
}

void Spectrograph::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QRect paintRect = rect();

    QColor startCol(20, 22, 54);
    QColor endCol(0, 0, 0);
    QLinearGradient gradient(paintRect.bottomLeft(), paintRect.topLeft());
    gradient.setColorAt(0, startCol);
    gradient.setColorAt(1, endCol);
    painter.setBrush(QBrush(gradient));
    painter.drawRect(paintRect);

    // TODO: draw grid with labels

    if (m_mode == MODE_LOG)
        drawSpectrum(painter, m_spectrumLog);
    else
        drawSpectrum(painter, m_spectrumMag);
}

void Spectrograph::drawSpectrum(QPainter& painter, const QVector<double>& spectrum)
{
    if (spectrum.size() == 0)
        return;

    QRect paintRect = rect();

    QPolygonF polygon;

    double minVal = qInf();
    double maxVal = 0;
    for (int i = 0; i < spectrum.size(); i++) {
        double val = spectrum[i];

        static int reflevel = -10;
        static int ampspan = 75;

        val = 255 * (val - reflevel) / ampspan;

        if (val < minVal)
            minVal = val;
        else if (val > maxVal)
            maxVal = val;

        polygon.append(QPointF(i, val));
    }
    polygon.prepend(QPointF(0, minVal));
    polygon.append(QPointF(polygon.size() - 1, minVal));

    double scaleX = paintRect.width() / (double)polygon.size();
    double scaleY = paintRect.height() / maxVal;
    painter.translate(0, paintRect.height());
    painter.scale(scaleX, -scaleY);

    QColor polyCol(114, 114, 135);
    painter.setBrush(QBrush(polyCol));

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(Qt::white));

    // Better performance: http://www.qtcentre.org/threads/23818-Performance-with-many-filled-polygons-on-screen
    // maybe draw to an offline pixmap
    painter.drawConvexPolygon(polygon);
}

void Spectrograph::resizeEvent(QResizeEvent*)
{
}

void Spectrograph::redraw()
{
    update();
}

void Spectrograph::addSpectrumLog(const QVector<double>& spectrum, double binSize, double maxFrq)
{
    m_spectrumLog = spectrum;
}

void Spectrograph::addSpectrumMag(const QVector<double>& spectrum, double binSize, double maxFrq)
{
    m_spectrumMag = spectrum;
}
