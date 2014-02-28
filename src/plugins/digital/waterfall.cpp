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
    : QWidget(parent),
      m_binSize(0),
      m_maxFrq(0),
      m_refLevel(-10),
      m_ampSpan(90),
      m_spectrumSize(0),
      m_showMarkers(true),
      m_lowerPassband(100),
      m_upperPassband(3000),
      m_drawOverlayFrq(true)
{
    m_frequency = 1000;
    m_bandwidth = 200;

    m_updateTimer = new QTimer(this);
    m_updateTimer->setTimerType(Qt::PreciseTimer);
    m_updateTimer->setInterval(30);   // time in ms
    connect(m_updateTimer, &QTimer::timeout, this, &Waterfall::redraw);
    m_updateTimer->start();
}

Waterfall::~Waterfall(void)
{
}

void Waterfall::init(int specSize)
{
    m_spectrumSize = specSize;

    // TODO: waterfallSize.height from x seconds

    // the waterfall pixmap has a fixed size independently from the window size
    QSize waterfallSize(m_spectrumSize, 1000);
    m_waterfallPixmap = QPixmap(waterfallSize);
    m_waterfallPixmap.fill(Qt::black);

    //ColormapBlue colormap;
    //ColormapCuteSDR colormap;
    ColormapFldigi colormap;
    m_colorMap = new Colormap(256, this);
    m_colorMap->accept(colormap);
    //m_colorMap->accept(colormap);

    setMouseTracking(true);
}

void Waterfall::reset()
{
    m_waterfallPixmap.fill(Qt::black);
}

void Waterfall::setLowerPass(double value)
{
    if (value < 0) {
        qWarning() << "value < 0, setting to 0";
        value = 0;
    }
    else if (value > m_upperPassband) {
        qWarning() << "value < m_upperPassband, setting to 0";
        value = 0;
    }

    m_lowerPassband = value;
}

void Waterfall::setUpperPass(double value)
{
    if (value > m_maxFrq) {
        qWarning() << "value > m_maxFrq, setting to m_maxFrq";
        value = m_maxFrq;
    }
    else if (value < m_lowerPassband) {
        qWarning() << "value < m_lowerPassband, setting to m_maxFrq";
        value = m_maxFrq;
    }

    m_upperPassband = value;
}

void Waterfall::setRefLevel(int value)
{
    m_refLevel = value;
}

void Waterfall::setAmpSpan(int value)
{
    m_ampSpan = value;
}

double Waterfall::getLowerPass() const
{
    return m_lowerPassband;
}

double Waterfall::getUpperPass() const
{
    return m_upperPassband;
}

int Waterfall::getRefLevel() const
{
    return m_refLevel;
}

int Waterfall::getAmpSpan() const
{
    return m_ampSpan;
}

void Waterfall::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QRect paintRect = rect();
    painter.fillRect(paintRect, Qt::black);

    // draw frequency overlay
    if (m_drawOverlayFrq)
        painter.drawPixmap(0, 0, m_overlayFrq);

    // draw and scale waterfall pixmap into widget painter
    QRect dest(0, m_overlayFrq.height(), paintRect.width(), m_waterfallPixmap.height());
    painter.drawPixmap(dest, m_waterfallPixmap);

    drawMarkers();
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawPixmap(0, 0, m_overlayMarkers);
}

void Waterfall::drawMarkers()
{
    // draw markers overlay
    QPainter painter(&m_overlayMarkers);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.fillRect(m_overlayMarkers.rect(), QColor(255, 255, 255, 0));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QColor frqCol(255, 255, 255, 255);

    // draw modem markers
    if (m_showMarkers) {
        QColor bwCol(255, 0, 0, 255);
        drawMarkers(painter, m_frequency, frqCol, bwCol);

        // draw mouse marker
        if (m_showMouse) {
            QColor bwCol(255, 255, 0, 255);
            drawMarkers(painter, m_mouseFrequency, frqCol, bwCol);
        }
    }
}

void Waterfall::resizeEvent(QResizeEvent*)
{
    if(!size().isValid())
        return;

    if(m_size != size()) {
        m_size = size();

        if (m_drawOverlayFrq) {
            m_overlayFrq = QPixmap(m_size.width(), 25);
            drawOverlay();
        }

        m_overlayMarkers = QPixmap(m_size);
        m_overlayMarkers.fill(Qt::transparent);
    }
}

void Waterfall::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF& pos = event->pos();

    qreal frequency = m_lowerPassband + (m_upperPassband - m_lowerPassband) * pos.x() / qreal(m_size.width());
    m_mouseFrequency = frequency;
    update();
}

void Waterfall::mousePressEvent(QMouseEvent* event)
{
    if (m_showMarkers) {
        Qt::MouseButton button = event->button();
        if (button == Qt::LeftButton) {
            QPoint pos = event->pos();

            // determine the clicked frequency
            double frequency = screenToFrq(pos.x());
            if (frequency < m_lowerPassband)
                frequency = m_lowerPassband;
            else if (frequency > m_upperPassband)
                frequency = m_upperPassband;

            m_frequency = frequency;
            emit frequencySelected(m_frequency);
        }
    }
}

void Waterfall::enterEvent(QEvent*)
{
    m_showMouse = true;
}

void Waterfall::leaveEvent(QEvent*)
{
    m_showMouse = false;
}

void Waterfall::redraw()
{
    // TODO: only when new data is available
    update();
}

qreal Waterfall::bwToScreen(double bw)
{
    return m_size.width() / (m_upperPassband - m_lowerPassband) * (bw);
}

qreal Waterfall::frqToScreen(double frq)
{
    return m_size.width() * (frq - m_lowerPassband) / (m_upperPassband - m_lowerPassband);
}

double Waterfall::screenToFrq(qreal pos)
{
    return m_lowerPassband + (m_upperPassband - m_lowerPassband) * pos / m_size.width();
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
    qreal upPos = m_overlayFrq.height();

    // determine the distance of the bandwidth markers
    qreal bwPos = bwToScreen(m_bandwidth);
    bwPos /= 2.0;

    // the width of the bandwidth gradients
    int bwSize = 10;

    /*int dimmedSize = (bwPos + bwSize) * 2 + 100;
    QRect dimmedRect(pos - dimmedSize / 2, upPos, dimmedSize, m_overlayMarkers.height());
    QLinearGradient dimmedGradient(dimmedRect.topRight(), dimmedRect.topLeft());
    dimmedGradient.setColorAt(0, QColor(0, 0, 0, 0));
    dimmedGradient.setColorAt(0.3, QColor(0, 0, 0, 100));
    dimmedGradient.setColorAt(0.7, QColor(0, 0, 0, 100));
    dimmedGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(dimmedGradient));
    painter.drawRect(dimmedRect);
    painter.setBrush(QBrush());*/

    // draw center frequency
    QPointF lineStart(pos, upPos);
    QPointF lineEnd(lineStart.x(), m_overlayMarkers.height());
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
    QRect upperGradientRect(pos + bwPos + 1, upPos, bwSize, m_overlayMarkers.height());
    QLinearGradient upperGradient(upperGradientRect.topLeft(), upperGradientRect.topRight());
    upperGradient.setColorAt(0, bwAlpha);
    upperGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(upperGradient));
    painter.setPen(Qt::NoPen);
    painter.drawRect(upperGradientRect);

    // draw lower bandwidth gradient
    QRect lowerGradientRect(pos - bwPos - bwSize, upPos, bwSize, m_overlayMarkers.height());
    QLinearGradient lowerGradient(lowerGradientRect.topRight(), lowerGradientRect.topLeft());
    lowerGradient.setColorAt(0, bwAlpha);
    lowerGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(lowerGradient));
    painter.drawRect(lowerGradientRect);
    painter.setBrush(QBrush());
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

void Waterfall::addSpectrum(const QVector<double>& spectrum, double binSize, double maxFrq)
{
    if (m_waterfallPixmap.isNull())
        return;

    if (m_binSize != binSize || m_maxFrq != maxFrq) {
        m_binSize = binSize;
        m_maxFrq = maxFrq;
        m_waterfallPixmap.fill(Qt::black);
        drawOverlay();
    }

    const int width = m_waterfallPixmap.width();
    const int height = m_waterfallPixmap.height();

    // scroll waterfall pixmap down by 1 pixel
    m_waterfallPixmap.scroll(0, 1, 0, 0, width, height);

    QPainter painter(&m_waterfallPixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int lower = m_lowerPassband / binSize;
    lower = lower < 0 ? 0 : lower;
    int upper = m_upperPassband / binSize;
    upper = upper > spectrum.size() ? spectrum.size() : upper;

    // set scaling to the spectrum's coordinate system
    double scaleX = width / (double)(upper - lower);
    painter.scale(scaleX, 1);
    painter.translate(-lower, 0);

    // draw fft data into pixmap in the spectrum's coordinate system
    for (int i = lower; i < upper; i++) {
        double value = spectrum[i];

        int intVal = log2disp(value);

        QColor col = m_colorMap->getColorValue(intVal);

        painter.setPen(col);
        painter.drawPoint(i, 0);
    }
}

void Waterfall::bandwidthChanged(double bandwidth)
{
    m_bandwidth = bandwidth;
}

void Waterfall::frequencyChanged(double frequency)
{
    m_frequency = frequency;
}

void Waterfall::modemActive(bool active)
{
    m_showMarkers = active;

    // hide or show the mouse cursor
    if (active) {
        setCursor(Qt::BlankCursor);
        emit frequencySelected(m_frequency);
    }
    else
        setCursor(Qt::ArrowCursor);
}

void Waterfall::drawOverlay()
{
    if (m_overlayFrq.isNull())
        return;

    const int width = m_overlayFrq.width();
    const int height = m_overlayFrq.height();

    m_overlayFrq.fill(Qt::black);

    QPainter painter(&m_overlayFrq);
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
}
