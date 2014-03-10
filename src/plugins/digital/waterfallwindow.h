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

#ifndef WATERFALLWINDOW_H
#define WATERFALLWINDOW_H

#include <QWidget>
#include <QStackedWidget>
#include <QThread>

namespace Digital {
namespace Internal {

class AudioDeviceList;
class AudioDeviceIn;
class Spectrum;
class WaterfallToolBar;
class Waterfall;
class Spectrograph;

class WaterfallWindow
        : public QWidget
{
    Q_OBJECT

public:
    enum WaterfallSpeed
    {
        WF_FAST,
        WF_NORMAL,
        WF_SLOW
    };

    WaterfallWindow(QWidget*);
    ~WaterfallWindow();

    void setSpeed(float speedMs);
    void reset();

public slots:
    void start(AudioDeviceIn*);
    void stop();
    void bandwidthChanged(double);
    void frequencyChanged(double);
    void modemActive(bool);
    void widgetSelected(int);

signals:
    void frequencySelected(double);

private:
    QStackedWidget*     m_widget;
    AudioDeviceIn*      m_inputDevice;
    Spectrum*           m_spectrum;
    WaterfallToolBar*   m_toolBar;
    Waterfall*          m_waterfall;
    Waterfall*          m_waterfallCombined;
    Spectrograph*       m_spectrograph;
    Spectrograph*       m_spectrographCombined;
    QTimer*             m_spectrumTimer;
    QThread*            m_timerThread;
};

} // namespace Internal
} // namespace Digital

#endif // WATERFALLWINDOW_H
