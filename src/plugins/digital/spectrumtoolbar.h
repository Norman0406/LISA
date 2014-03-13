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

#ifndef SPECTRUMTOOLBAR_H
#define SPECTRUMTOOLBAR_H

#include <utils/styledbar.h>
#include <QComboBox>
#include <QLabel>

namespace Digital {
namespace Internal {

class SpectrumToolBar
        : public Utils::StyledBar
{
    Q_OBJECT

public:
    SpectrumToolBar(QWidget*);

    void addSpectrumWidget(QString);

public slots:
    void mouseMoved(double);
    void mouseVisible(bool);

signals:
    void widgetSelected(int);

private:
    QComboBox* m_cmbSpecType;
    QLabel* m_lblFrqText;
    QLabel* m_lblFrq;
    QLabel* m_lblInfo;
};

} // namespace Internal
} // namespace Digital

#endif // SPECTRUMTOOLBAR_H
