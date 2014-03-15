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

#ifndef MESSENGERTOOLBAR_H
#define MESSENGERTOOLBAR_H

#include <utils/styledbar.h>

#include <QToolButton>
#include <QComboBox>
#include <QLabel>

namespace Digital {
namespace Internal {

class MessengerToolBar
        : public Utils::StyledBar
{
    Q_OBJECT

public:
    MessengerToolBar(QWidget*);

public slots:
    void frequencyChanged(double);

private slots:
    void modemIndexChanged(int);
    void txStarted();
    void txStopped();

signals:
    void startTX();
    void stopTX();
    void clear();
    void modemSelected(QString);

protected:
    bool event(QEvent*);

private:
    QToolButton* m_btTxStart;
    QToolButton* m_btTxStop;
    QToolButton* m_btClear;
    QComboBox* m_cmbModems;
    QComboBox* m_cmbConfig;
    QLabel* m_lblFrq;
};

} // namespace Internal
} // namespace Digital

#endif // MESSENGERTOOLBAR_H
