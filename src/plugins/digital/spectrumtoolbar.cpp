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

#include "spectrumtoolbar.h"
#include <QHBoxLayout>

using namespace Digital::Internal;

SpectrumToolBar::SpectrumToolBar(QWidget* parent)
    : Utils::StyledBar(parent)
{
    QHBoxLayout* toolBarLayout = new QHBoxLayout(this);
    toolBarLayout->setMargin(0);
    toolBarLayout->setSpacing(0);

    m_cmbSpecType = new QComboBox(this);
    connect(m_cmbSpecType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(widgetSelected(int)));
    toolBarLayout->addWidget(m_cmbSpecType);

    m_lblFrqText = new QLabel(QString(tr("Frequency:")));
    m_lblFrqText->setMargin(5);
    toolBarLayout->addWidget(m_lblFrqText);
    m_lblFrq = new QLabel(QLatin1String("0 Hz"));
    toolBarLayout->addWidget(m_lblFrq);
    mouseVisible(false);

    toolBarLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    m_lblInfo = new QLabel(QString::fromLatin1("Sound Info"), this);
    toolBarLayout->addWidget(m_lblInfo);
}

void SpectrumToolBar::addSpectrumWidget(QString name)
{
    m_cmbSpecType->addItem(name);
}

void SpectrumToolBar::mouseMoved(double frq)
{
    // TODO: show MHz when connected to a radio
    m_lblFrq->setText(QString::number(frq) + QLatin1String(" Hz"));
}

void SpectrumToolBar::mouseVisible(bool visible)
{
    if (visible) {
        m_lblFrqText->setVisible(true);
        m_lblFrq->setVisible(true);
    }
    else {
        m_lblFrqText->setVisible(false);
        m_lblFrq->setVisible(false);
    }
}
