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

#include <QWidget>
#include <QTimer>

namespace Digital {
namespace Internal {

class Colormap;

class Waterfall
        : public QWidget    // : public SpectrumWidget
{
    Q_OBJECT

public:
    explicit Waterfall(QWidget* parent);
    ~Waterfall(void);

    void init(int);
    void reset();

    void setLowerPass(double);
    void setUpperPass(double);
    void setRefLevel(int);
    void setAmpSpan(int);

    double getLowerPass() const;
    double getUpperPass() const;
    int getRefLevel() const;
    int getAmpSpan() const;

signals:
    void frequencySelected(double);

public slots:
    void addSpectrum(const QVector<double>&, double, double);
    void bandwidthChanged(double);
    void frequencyChanged(double);
    void modemActive(bool);    // a modem has been selected, lock the frequency

private slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void drawOverlay();

private:
    qreal bwToScreen(double);
    qreal frqToScreen(double);
    double screenToFrq(qreal);
    void drawMarkers();
    void drawMarkers(QPainter&, qreal, const QColor&, const QColor&);
    int log2disp(double);

    QTimer*     m_updateTimer;

    double      m_binSize;
    double      m_maxFrq;
    int         m_refLevel;
    int         m_ampSpan;
    int         m_spectrumSize;
    QSize       m_size;
    QPixmap     m_waterfallPixmap;
    QPixmap     m_overlayFrq;
    QPixmap     m_overlayMarkers;
    float       m_mouseFrequency;

    // modem specific
    float       m_frequency;
    float       m_bandwidth;
    bool        m_showMarkers;

    // TODO: use either of them
    Colormap*	m_colorMap;
    QColor		m_colorTable[256];

    double		m_lowerPassband;
    double      m_upperPassband;
    bool		m_drawOverlayFrq;
    bool        m_showMouse;
};

} // namespace Internal
} // namespace Digital

#endif // WATERFALL_H
