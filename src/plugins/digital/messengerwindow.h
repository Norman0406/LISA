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

#ifndef MESSENGERWINDOW_H
#define MESSENGERWINDOW_H

#include <QWidget>
#include <QThread>

namespace Digital {
namespace Internal {

class MessengerToolBar;
class Messenger;
class AudioDeviceIn;
class AudioDeviceOut;
class ModemManager;

class MessengerWindow
        : public QWidget
{
    Q_OBJECT

public:
    MessengerWindow(QWidget*);
    ~MessengerWindow();

    ModemManager* getModemManager();

public slots:
    void inDeviceReady(AudioDeviceIn*);
    void outDeviceReady(AudioDeviceOut*);
    void received(int, char);

private slots:
    void modemSelected(QString);

signals:
    void modemActive(bool);

private:
    MessengerToolBar*   m_toolBar;
    Messenger*          m_messenger;
    ModemManager*       m_modemManager;
};

} // namespace Internal
} // namespace Digital

#endif // MESSENGERWINDOW_H
