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

#include "waterfall.h"
#include "colormap.h"
#include "colormapvisitor.h"
#include <QtGui/QPainter>
#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QLinearGradient>

#include <QDebug>

using namespace Digital::Internal;

Waterfall::Waterfall(QWidget* parent)
    : SpectrumWidget(parent),
      m_refLevel(-10),
      m_ampSpan(90),
      m_scrollBuffer(0),
      m_scrollPosition(0),
      m_colorMap(0)
{
}

Waterfall::~Waterfall(void)
{
    delete[] m_scrollBuffer;
}

void Waterfall::iInit()
{
    delete m_colorMap;

    //ColormapBlue colormap;
    //ColormapCuteSDR colormap;
    ColormapFldigi colormap;
    m_colorMap = new Colormap(256, this);
    m_colorMap->accept(colormap);
    //m_colorMap->accept(colormap);
}

void Waterfall::reset()
{
    m_waterfall.fill(Qt::black);
    m_scrollPosition = 0;
}

void Waterfall::setRefLevel(int value)
{
    m_refLevel = value;
}

void Waterfall::setAmpSpan(int value)
{
    m_ampSpan = value;
}

int Waterfall::getRefLevel() const
{
    return m_refLevel;
}

int Waterfall::getAmpSpan() const
{
    return m_ampSpan;
}

void Waterfall::beginDraw(QPainter& painter)
{
    QRect paintRect = rect();
    painter.fillRect(paintRect, Qt::black);
}

void Waterfall::drawSpectrum(QPainter& painter, const QRect& rect)
{
    QRect newRect(rect);
    newRect.setHeight(m_waterfall.height());
    painter.drawImage(newRect, m_waterfall);
}

QRect Waterfall::drawFrequencies(QPainter& painter)
{
    if (m_frequencies.isNull())
        return QRect();

    painter.drawPixmap(0, 0, m_frequencies);
    return QRect(m_frequencies.rect());
}

void Waterfall::drawFrequencies()
{
    if (!m_frequencies.isNull()) {
        const int width = m_frequencies.width();
        const int height = m_frequencies.height();

        m_frequencies.fill(Qt::black);

        QPainter painter(&m_frequencies);
        painter.setPen(Qt::yellow);

        const double bigLineFrq = 500.0;	// lines every x Hz

        // set font
        QFont font = painter.font();
        font.setPixelSize(height / 2);
        painter.setFont(font);

        const int mainLinePos = height - 1;
        painter.drawLine(0, mainLinePos, width, mainLinePos);

        for (double frq = 0; frq < m_maxFrq; frq += bigLineFrq) {
            if (frq > m_lowerPassband && frq < m_upperPassband) {

                qreal pos = frqToScreen(frq);
                QString text = QString::number(frq);
                int textWidth = painter.fontMetrics().width(text);
                int textHeight = painter.fontMetrics().height();

                if (pos - textWidth / 2.0 < 0 ||
                        pos + textWidth / 2.0 > width)
                    continue;

                painter.drawText(pos - textWidth / 2.0, textHeight, text);
                painter.drawLine(pos, textHeight + 5, pos, height);
            }
        }
        requestRedraw();
    }
}

void Waterfall::drawMarkers(QPainter& painter, qreal frequency, const QColor& frqCol, const QColor& bwCol)
{
    // check and limit the actual marker frequency to the possible frequency range
    if (frequency < m_lowerPassband)
        frequency = m_lowerPassband;
    else if (frequency > m_upperPassband)
        frequency = m_upperPassband;

    qreal pos = frqToScreen(frequency);

    // the position where the markers start
    qreal upPos = m_frequencies.height();

    // determine the distance of the bandwidth markers
    qreal bwPos = bwToScreen(m_bandwidth);
    bwPos /= 2.0;

    // the width of the bandwidth gradients
    int bwSize = 10;

    // draw center frequency
    QPointF lineStart(pos, upPos);
    QPointF lineEnd(lineStart.x(), m_size.height());
    painter.setPen(frqCol);
    painter.drawLine(lineStart, lineEnd);

    // draw upper bandwidth limit
    lineStart.setX(pos + bwPos);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    // draw lower bandwidth limit
    lineStart.setX(pos - bwPos);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    QColor bwAlpha = bwCol;
    bwAlpha.setAlpha(50);

    // draw upper bandwidth gradient
    QRect upperGradientRect(pos + bwPos + 1, upPos, bwSize, m_size.height());
    QLinearGradient upperGradient(upperGradientRect.topLeft(), upperGradientRect.topRight());
    upperGradient.setColorAt(0, bwAlpha);
    upperGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(upperGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRect(upperGradientRect);

    // draw lower bandwidth gradient
    QRect lowerGradientRect(pos - bwPos - bwSize, upPos, bwSize, m_size.height());
    QLinearGradient lowerGradient(lowerGradientRect.topRight(), lowerGradientRect.topLeft());
    lowerGradient.setColorAt(0, bwAlpha);
    lowerGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(lowerGradient));
    painter.drawRect(lowerGradientRect);

    // draw bandwidth alpha
    /*QRect bwAlphaRect(pos - bwPos + 1, upPos, bwPos * 2 - 1, m_size.height());
    painter.setBrush(QBrush(QColor(255, 255, 255, 50)));
    painter.drawRect(bwAlphaRect);*/
}

void Waterfall::sizeChanged(const QSize& size)
{
    QSize waterfallSize = size;
    if (m_showFrequencies) {
        m_frequencies = QPixmap(size.width(), 25);
        waterfallSize.setHeight(size.height() - m_frequencies.height());
        drawFrequencies();
    }

    m_waterfall = QImage(waterfallSize, QImage::Format_RGB32);
    m_scrollPosition = 0;

    delete[] m_scrollBuffer;
    m_scrollBuffer = new QRgb[m_waterfall.width() * m_waterfall.height()];
}

int Waterfall::log2disp(double value)
{
    value = 255.0 * (m_refLevel - value) / m_ampSpan;

    if (value < 0)
        return 255;
    if (value > 255 )
        return 0;

    return (int)(255 - value);
}

void Waterfall::redraw()
{
    if (m_waterfall.isNull() || m_recentData.size() == 0)
        return;

    // At the time of redraw(), several spectrums may have been computed and added to
    // m_recentData. During redraw(), the waterfall image is updated such that existing data
    // is moved down (scrolled) and the most recent data available in m_recentData is added
    // on top of the waterfall image. Scrolling is done by using a temporary scrolling buffer.
    // In order to achieve the necessary performance, scrolling is done on the raw pixel data
    // instead of using a QPainter.

    // get the data size that needs to be scrolled
    int scrollSize = m_waterfall.bytesPerLine() * m_scrollPosition;
    int availableSize = m_waterfall.bytesPerLine() * (m_waterfall.height() - m_recentData.size());
    if (scrollSize > availableSize)
        scrollSize = availableSize;

    // scroll image
    if (m_scrollPosition > 0) {
        QRgb* srcData = (QRgb*)m_waterfall.scanLine(0);
        QRgb* dstData = (QRgb*)m_waterfall.scanLine(m_recentData.size());
        memcpy(m_scrollBuffer, srcData, scrollSize);
        memcpy(dstData, m_scrollBuffer, scrollSize);
    }

    int lower = m_lowerPassband / m_binSize;
    lower = lower < 0 ? 0 : lower;
    int upper = m_upperPassband / m_binSize;
    const int spectrumSize = m_recentData.first().size();
    upper = upper > spectrumSize ? spectrumSize : upper;

    // draw fft data into pixmap in the spectrum's coordinate system
    for (int i = 0; i < m_recentData.size(); i++) {
        QRgb* data = (QRgb*)m_waterfall.scanLine(i);
        const QVector<double>& spectrum = m_recentData[i];
        for (int j = 0; j < m_waterfall.width(); j++) {
            int index = ((j / (double)m_waterfall.width()) * (upper - lower)) + lower;
            const double& value = spectrum[index];

            int intVal = log2disp(value);
            QColor col = m_colorMap->getColorValue(intVal);

            data[j] = qRgb(col.red(), col.green(), col.blue());
        }
    }

    if (m_scrollPosition != m_waterfall.height() &&
            m_scrollPosition + m_recentData.size() > m_waterfall.height())
        m_scrollPosition = m_waterfall.height();
    else if (m_scrollPosition + m_recentData.size() < m_waterfall.height())
        m_scrollPosition += m_recentData.size();

    m_recentData.clear();

    requestRedraw();

    SpectrumWidget::redraw();
}

void Waterfall::iAddSpectrumLog(const QVector<double>& spectrum, bool changed)
{
    // store the newly computed spectrum
    m_recentData.push_front(spectrum);

    if (changed) {
        reset();
        //drawFrequencies();
    }
}
