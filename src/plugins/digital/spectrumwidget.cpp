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

#include "spectrumwidget.h"
#include <QMouseEvent>
#include <QDebug>

using namespace Digital::Internal;

SpectrumWidget::SpectrumWidget(QWidget* parent)
    : QWidget(parent),
      m_binSize(0),
      m_maxFrq(0),
      m_spectrumSize(0),
      m_frequency(1000),
      m_bandwidth(100),
      m_mouseFrequency(1000),
      m_showMouse(false),
      m_showMarkers(false),
      m_showFrequencies(true),
      m_lowerPassband(0),
      m_upperPassband(4000),
      m_refLevel(-10),
      m_ampSpan(100),
      m_needToRedraw(false)
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setTimerType(Qt::PreciseTimer);
    m_updateTimer->setInterval(30);   // time in ms
    connect(m_updateTimer, &QTimer::timeout, this, &SpectrumWidget::redraw);

    m_size = size();
}

SpectrumWidget::~SpectrumWidget()
{
}

void SpectrumWidget::init(int specSize)
{
    m_spectrumSize = specSize;
    setMouseTracking(true);

    iInit();

    m_updateTimer->start();
}

void SpectrumWidget::reset()
{
    // not implemented
}

void SpectrumWidget::setLowerPass(double value)
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

void SpectrumWidget::setUpperPass(double value)
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

void SpectrumWidget::setRefLevel(int value)
{
    m_refLevel = value;
}

void SpectrumWidget::setAmpSpan(int value)
{
    m_ampSpan = value;
}

double SpectrumWidget::getLowerPass() const
{
    return m_lowerPassband;
}

double SpectrumWidget::getUpperPass() const
{
    return m_upperPassband;
}

int SpectrumWidget::getRefLevel() const
{
    return m_refLevel;
}

int SpectrumWidget::getAmpSpan() const
{
    return m_ampSpan;
}

void SpectrumWidget::addSpectrumLog(const QVector<double>& spectrum, double binSize, double maxFrq)
{
    if (!isVisible())
        return;

    bool changed = false;

    if (m_binSize != binSize || m_maxFrq != maxFrq) {
        m_binSize = binSize;
        m_maxFrq = maxFrq;
        changed = true;
    }

    iAddSpectrumLog(spectrum, changed);
}

void SpectrumWidget::bandwidthChanged(double bandwidth)
{
    if (bandwidth != m_bandwidth) {
        m_bandwidth = bandwidth;
        drawMarkers();
    }
}

void SpectrumWidget::frequencyChanged(double frequency)
{
    if (frequency != m_frequency) {
        m_frequency = frequency;
        drawMarkers();
        drawFrequencies();
    }
}

void SpectrumWidget::moveMouse(double frequency)
{
    m_mouseFrequency = frequency;
    drawMarkers();
    update();
}

void SpectrumWidget::showMouse(bool show)
{
    m_showMouse = show;
    drawMarkers();
    update();
}

void SpectrumWidget::modemActive(bool active)
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

void SpectrumWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // TODO: only draw what has changed

    // begin drawing, i.e. draw background etc
    beginDraw(painter);

    QRect spectrumRect = rect();

    // draw overlay
    if (m_showFrequencies) {
        QRect frqRect = drawFrequencies(painter);

        // adjust spectrum rectangle
        spectrumRect.setTop(frqRect.bottom() + 1);
    }

    // draw actual spectrum data
    drawSpectrum(painter, spectrumRect);

    // draw modem markers
    if (m_showMarkers || m_showMouse) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(0, 0, m_markers);
    }
}

void SpectrumWidget::drawMarkers()
{
    if (!m_showMarkers && !m_showMouse)
        return;

    if (!isVisible())
        return;

    if (!m_markers.isNull()) {
        // draw markers overlay
        QPainter painter(&m_markers);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.fillRect(m_markers.rect(), QColor(255, 255, 255, 0));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        QColor frqCol(255, 255, 255, 255);
        QColor bwCol(255, 0, 0, 255);

        // draw modem marker
        if (m_showMarkers) {
            drawMarkers(painter, m_frequency, frqCol, bwCol);
            requestRedraw();
        }

        // draw mouse marker
        if (m_showMouse) {
            bwCol = QColor(255, 255, 0, 255);

            // draw mouse marker
            drawMarkers(painter, m_mouseFrequency, frqCol, bwCol);

            update();
        }
    }
}

void SpectrumWidget::resizeEvent(QResizeEvent*)
{
    if(!size().isValid())
        return;

    if(m_size != size()) {
        m_size = size();

        if (m_showMarkers || m_showMouse) {
            m_markers = QPixmap(m_size);
            m_markers.fill(Qt::transparent);
            drawMarkers();
        }

        sizeChanged(m_size);
    }
}

void SpectrumWidget::iAddSpectrumLog(const QVector<double>&, bool)
{
    // not implemented
}

void SpectrumWidget::sizeChanged(const QSize&)
{
    // not implemented
}

void SpectrumWidget::iRedraw()
{
    // not implemented
}

void SpectrumWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_showMouse) {
        const QPointF& pos = event->pos();

        qreal frequency = m_lowerPassband + (m_upperPassband - m_lowerPassband) * pos.x() / qreal(m_size.width());
        m_mouseFrequency = frequency;

        moveMouse(frequency);
        emit mouseMoved(frequency);
    }
}

void SpectrumWidget::mousePressEvent(QMouseEvent* event)
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

            drawMarkers();
            update();
        }
    }
}

void SpectrumWidget::enterEvent(QEvent*)
{
    showMouse(true);
    emit mouseVisible(true);
}

void SpectrumWidget::leaveEvent(QEvent*)
{
    showMouse(false);
    emit mouseVisible(false);
}

void SpectrumWidget::requestRedraw()
{
    m_needToRedraw = true;
}

void SpectrumWidget::redraw()
{
    if (m_needToRedraw && isVisible()) {
        iRedraw();
        update();
        m_needToRedraw = false;
    }
}

qreal SpectrumWidget::bwToScreen(double bandwidth)
{
    return m_size.width() / (m_upperPassband - m_lowerPassband) * (bandwidth);
}

qreal SpectrumWidget::frqToScreen(double frequency)
{
    return m_size.width() * (frequency - m_lowerPassband) / (m_upperPassband - m_lowerPassband);
}

double SpectrumWidget::screenToFrq(qreal pos)
{
    return m_lowerPassband + (m_upperPassband - m_lowerPassband) * pos / m_size.width();
}
