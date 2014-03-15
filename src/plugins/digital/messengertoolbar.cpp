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

#include "messengertoolbar.h"

#include "digitalconstants.h"
#include "factory.h"
#include "modems/modem.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QAction>
#include <QEvent>
#include <QWidgetAction>

using namespace Digital::Internal;

MessengerToolBar::MessengerToolBar(QWidget* parent)
    : Utils::StyledBar(parent)
{
    QHBoxLayout* toolBarLayout = new QHBoxLayout(this);
    toolBarLayout->setMargin(0);
    toolBarLayout->setSpacing(0);

    QAction* actTxStart = new QAction(QIcon(QLatin1String(Constants::ICON_START_TX)), tr("Start TX"), this);
    m_btTxStart = new QToolButton(this);
    m_btTxStart->setDefaultAction(actTxStart);
    toolBarLayout->addWidget(m_btTxStart);
    connect(actTxStart, &QAction::triggered, this, &MessengerToolBar::startTX);
    //connect(actTxStart, &QAction::triggered, this, &MessengerToolBar::txStarted);

    QAction* actTxStop = new QAction(QIcon(QLatin1String(Constants::ICON_STOP_TX)), tr("Stop TX"), this);
    m_btTxStop = new QToolButton(this);
    m_btTxStop->setDefaultAction(actTxStop);
    toolBarLayout->addWidget(m_btTxStop);
    connect(actTxStop, &QAction::triggered, this, &MessengerToolBar::stopTX);
    //connect(actTxStop, &QAction::triggered, this, &MessengerToolBar::txStopped);

    QAction* actClear = new QAction(QIcon(QLatin1String(Constants::ICON_CLEAR)), tr("Clear"), this);
    m_btClear = new QToolButton(this);
    m_btClear->setDefaultAction(actClear);
    toolBarLayout->addWidget(m_btClear);
    connect(actClear, &QAction::triggered, this, &MessengerToolBar::clear);

    m_cmbModems = new QComboBox(this);
    m_cmbModems->addItem(tr("<Modem>"));
    m_cmbModems->addItems(Factory<Modem>::enumerateTypes());
    m_cmbModems->setCurrentIndex(-1);
    connect(m_cmbModems, SIGNAL(currentIndexChanged(int)), this, SLOT(modemIndexChanged(int)));
    toolBarLayout->addWidget(m_cmbModems);

    m_cmbConfig = new QComboBox(this);
    m_cmbConfig->addItem(tr("<Configuration>"));
    //m_cmbConfig->addItems(Factory<Modem>::enumerateTypes());
    //connect(m_cmbModems, SIGNAL(currentIndexChanged(int)), this, SLOT(modemIndexChanged(int)));
    toolBarLayout->addWidget(m_cmbConfig);

    //ConfigAction* act = new ConfigAction(m_cmbConfig);

    QLabel* lblFrqText = new QLabel(QString(tr("Frequency:")));
    lblFrqText->setMargin(5);
    toolBarLayout->addWidget(lblFrqText);
    m_lblFrq = new QLabel(QLatin1String("0 Hz"));
    toolBarLayout->addWidget(m_lblFrq);

    toolBarLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
}

bool MessengerToolBar::event(QEvent* event)
{
    if (event->type() == QEvent::Polish) {
        m_cmbModems->setCurrentIndex(1);    // set initial modem choice
    }

    return QWidget::event(event);
}

void MessengerToolBar::modemIndexChanged(int index)
{
    if (index <= 0) {
        m_btTxStart->setEnabled(false);
        m_btTxStop->setEnabled(false);
        m_btClear->setEnabled(false);

        // empty QString means no modem selected
        emit clear();
        emit modemSelected(QString());
    }
    else if (index > 0) {
        m_btTxStart->setEnabled(true);
        m_btTxStop->setEnabled(false);
        m_btClear->setEnabled(true);

        QString modemType = m_cmbModems->currentText();
        emit modemSelected(modemType);
    }
}

void MessengerToolBar::txStarted()
{
    m_btTxStart->setEnabled(false);
    m_btTxStop->setEnabled(true);
}

void MessengerToolBar::txStopped()
{
    m_btTxStart->setEnabled(true);
    m_btTxStop->setEnabled(false);
}

void MessengerToolBar::frequencyChanged(double frq)
{
    // TODO: show MHz when connected to a radio
    m_lblFrq->setText(QString::number(frq) + QLatin1String(" Hz"));
}
