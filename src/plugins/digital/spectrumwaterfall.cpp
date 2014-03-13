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

#include "spectrumwaterfall.h"
#include "colormap.h"
#include "colormapvisitor.h"
#include <QtGui/QPainter>
#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QLinearGradient>

#include <QDebug>

using namespace Digital::Internal;

SpectrumWaterfall::SpectrumWaterfall(QWidget* parent)
    : SpectrumWidget(parent),
      m_scrollBuffer(0),
      m_scrollPosition(0),
      m_fontSize(11),
      m_fontBorder(5),
      m_fontColor(Qt::yellow),
      m_colorMap(0)
{
}

SpectrumWaterfall::~SpectrumWaterfall(void)
{
    delete[] m_scrollBuffer;
}

void SpectrumWaterfall::iInit()
{
    delete m_colorMap;

    //ColormapBlue colormap;
    //ColormapCuteSDR colormap;
    ColormapFldigi colormap;
    m_colorMap = new Colormap(256, this);
    m_colorMap->accept(colormap);
    //m_colorMap->accept(colormap);
}

void SpectrumWaterfall::reset()
{
    m_waterfall.fill(Qt::black);
    m_scrollPosition = 0;
}

void SpectrumWaterfall::paint(QPainter& painter)
{
    painter.setBackgroundMode(Qt::OpaqueMode);

    if (m_frequencies.isNull()) {
        painter.drawImage(0, 0, m_waterfall);
    }
    else {
        painter.drawPixmap(0, 0, m_frequencies);
        painter.drawImage(0, m_frequencies.height(), m_waterfall);
    }
}

void SpectrumWaterfall::drawFrequencies()
{
    if (!m_frequencies.isNull()) {

        m_frequencies.fill(Qt::black);
        QPainter painter(&m_frequencies);

        // set font
        QFont font = painter.font();
        font.setPixelSize(m_fontSize);
        painter.setFont(font);
        const int fontAscent = painter.fontMetrics().ascent();
        const int fontHeight = painter.fontMetrics().height();

        QVector<int> frqSteps;
        frqSteps.push_back(100);
        frqSteps.push_back(200);
        frqSteps.push_back(500);
        frqSteps.push_back(1000);

        // determine space between two frequency lines
        int frqStep = frqSteps.last();
        for (int i = 0; i < frqSteps.size(); i++) {

            double maxWidth = 0;
            int count = 0;
            for (int j = 0; j < m_upperPassband; j += frqSteps[i]) {
                if (j > m_lowerPassband) {
                    QString text = QString::number(j);
                    double halfWidth = painter.fontMetrics().width(text) / 2.0;
                    maxWidth += halfWidth + m_fontBorder;
                    count++;
                }
            }
            maxWidth = (maxWidth * 2) / count;

            int lineSpace = frqToScreen(frqSteps[i]);
            if (lineSpace > maxWidth) {
                frqStep = frqSteps[i];
                break;
            }
        }

        // draw frequency labels
        for (int i = 0; i < m_upperPassband; i += frqStep) {
            if (i > m_lowerPassband) {
                double frq = frqToScreen(i);
                QString text = QString::number(i);
                double pos = frq - painter.fontMetrics().width(text) / 2.0;

                // draw text
                painter.setPen(m_fontColor);
                painter.drawText(QPoint(pos, fontAscent + m_fontBorder), text);

                // draw line
                QPointF start(frq, m_fontBorder * 2 + fontHeight);
                QPointF end(frq, m_frequencies.height() - 2);
                painter.drawLine(start, end);
            }
        }

        // draw end line
        QPointF start(0, m_frequencies.height() - 1);
        QPointF end(m_frequencies.width(), m_frequencies.height() - 1);
        painter.drawLine(start, end);
    }
}

void SpectrumWaterfall::drawMarkers(QPainter& painter, qreal frequency, const QColor& frqCol, const QColor& bwCol)
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
    qreal bwScreen = bwToScreen(m_bandwidth);
    bwScreen /= 2.0;

    // the width of the bandwidth gradients
    int bwSize = 10;

    // draw center frequency
    QPointF lineStart(pos, upPos);
    QPointF lineEnd(lineStart.x(), m_size.height());
    painter.setPen(frqCol);
    painter.drawLine(lineStart, lineEnd);

    // draw upper bandwidth limit
    lineStart.setX(pos + bwScreen);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    // draw lower bandwidth limit
    lineStart.setX(pos - bwScreen);
    lineEnd.setX(lineStart.x());
    painter.setPen(bwCol);
    painter.drawLine(lineStart, lineEnd);

    QColor bwAlpha = bwCol;
    bwAlpha.setAlpha(50);

    // draw upper bandwidth gradient
    QRectF upperGradientRect(pos + bwScreen - bwSize, upPos, bwSize, m_size.height());
    QLinearGradient upperGradient(upperGradientRect.topLeft(), upperGradientRect.topRight());
    upperGradient.setColorAt(0, QColor(0, 0, 0, 0));
    upperGradient.setColorAt(1, bwAlpha);
    painter.setBrush(QBrush(upperGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRect(upperGradientRect);

    // draw lower bandwidth gradient
    QRectF lowerGradientRect(pos - bwScreen, upPos, bwSize, m_size.height());
    QLinearGradient lowerGradient(lowerGradientRect.topRight(), lowerGradientRect.topLeft());
    lowerGradient.setColorAt(0, QColor(0, 0, 0, 0));
    lowerGradient.setColorAt(1, bwAlpha);
    painter.setBrush(QBrush(lowerGradient));
    painter.drawRect(lowerGradientRect);
}

void SpectrumWaterfall::sizeChanged(const QSize& size)
{
    QSize waterfallSize = size;
    m_frequencies = QPixmap(size.width(), 30);
    waterfallSize.setHeight(size.height() - m_frequencies.height());
    drawFrequencies();

    m_waterfall = QImage(waterfallSize, QImage::Format_RGB32);
    m_waterfall.fill(Qt::black);

    m_scrollPosition = 0;

    delete[] m_scrollBuffer;
    m_scrollBuffer = new QRgb[m_waterfall.width() * m_waterfall.height()];
}

int SpectrumWaterfall::log2disp(double value)
{
    value = 255.0 * (m_refLevel - value) / m_ampSpan;

    if (value < 0)
        return 255;
    if (value > 255 )
        return 0;

    return (int)(255 - value);
}

void SpectrumWaterfall::iRedraw()
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

    // compute array indices for lower and upper passband frequencies
    int lower = m_lowerPassband / m_binSize;
    lower = lower < 0 ? 0 : lower;
    int upper = m_upperPassband / m_binSize;
    const int spectrumSize = m_recentData.first().size();
    upper = upper > spectrumSize ? spectrumSize : upper;

    // draw the most recent data on top of the waterfall
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

    m_scrollPosition += m_recentData.size();
    if (m_scrollPosition > m_waterfall.height())
        m_scrollPosition = m_waterfall.height();

    m_recentData.clear();
}

void SpectrumWaterfall::iAddSpectrumLog(const QVector<double>& spectrum, bool changed)
{
    // store the newly computed spectrum
    m_recentData.push_front(spectrum);

    if (changed) {
        reset();
    }

    requestRedraw();
}
