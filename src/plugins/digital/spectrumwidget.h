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
#include <QTimer>
#include <QPainter>

namespace Digital {
namespace Internal {

class SpectrumWidget
        : public QWidget
{
    Q_OBJECT

signals:
    void frequencySelected(double);
    void mouseMoved(double);
    void mouseVisible(bool);

public:
    ~SpectrumWidget();

    void init(int);
    virtual void reset();

    void setLowerPass(double);
    void setUpperPass(double);
    void setRefLevel(int);
    void setAmpSpan(int);

    double getLowerPass() const;
    double getUpperPass() const;
    int getRefLevel() const;
    int getAmpSpan() const;

public slots:
    void addSpectrumLog(const QVector<double>&, double, double);
    void bandwidthChanged(double);
    void frequencyChanged(double);
    void moveMouse(double);
    void showMouse(bool);
    void modemActive(bool);    // a modem has been selected, lock the frequency

protected:
    SpectrumWidget(QWidget*);

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

    void requestRedraw();
    qreal bwToScreen(double);
    qreal frqToScreen(double);
    double screenToFrq(qreal);

    virtual void iInit() = 0;
    virtual void iAddSpectrumLog(const QVector<double>&, bool);
    virtual void sizeChanged(const QSize&);
    virtual void iRedraw();

    virtual void beginDraw(QPainter&) = 0;
    virtual void drawSpectrum(QPainter&, const QRect&) = 0;
    virtual QRect drawFrequencies(QPainter&) = 0;
    virtual void drawFrequencies() = 0;
    virtual void drawMarkers(QPainter&, qreal, const QColor&, const QColor&) = 0;

    double      m_binSize;
    double      m_maxFrq;
    int         m_spectrumSize;
    float       m_frequency;
    float       m_bandwidth;
    float       m_mouseFrequency;
    bool        m_showMouse;
    bool        m_showMarkers;
    bool        m_showFrequencies;
    double		m_lowerPassband;
    double      m_upperPassband;
    double      m_refLevel;
    double      m_ampSpan;
    QSize       m_size;
    QTimer*     m_updateTimer;
    QPixmap     m_markers;

private slots:
    virtual void redraw();

private:
    void drawMarkers();

    bool m_needToRedraw;
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMWIDGET_H
