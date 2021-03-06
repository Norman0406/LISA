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

#ifndef MESSENGER_H
#define MESSENGER_H

#include <QWidget>
#include <QTimer>

namespace Digital {
namespace Internal {

namespace Ui {
    class Messenger;
}

class Messenger
        : public QWidget
{
    Q_OBJECT

public:
    Messenger(QWidget*);

public slots:
    void modemSelected(QString);
    void clear();
    void received(char);
    void sent(char);
    void updateReceived();
    void updateSent();

private:
    Ui::Messenger* m_ui;

    QString m_received;
    QTimer* m_receivedTimer;
    bool m_receivedChanged;

    QString m_sent;
    QTimer* m_sentTimer;
    bool m_sentChanged;
};

} // namespace Internal
} // namespace Digital

#endif // MESSENGER_H
