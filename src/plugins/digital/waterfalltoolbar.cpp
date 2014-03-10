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

#include "waterfalltoolbar.h"
#include <QHBoxLayout>

using namespace Digital::Internal;

WaterfallToolBar::WaterfallToolBar(QWidget* parent)
    : Utils::StyledBar(parent)
{
    QHBoxLayout* toolBarLayout = new QHBoxLayout(this);
    toolBarLayout->setMargin(0);
    toolBarLayout->setSpacing(0);

    m_cmbSpecType = new QComboBox(this);
    connect(m_cmbSpecType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(widgetSelected(int)));
    toolBarLayout->addWidget(m_cmbSpecType);

    toolBarLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    m_lblInfo = new QLabel(QString::fromAscii("Sound Info"), this);
    toolBarLayout->addWidget(m_lblInfo);
}

void WaterfallToolBar::addSpectrumWidget(QString name)
{
    m_cmbSpecType->addItem(name);
}
