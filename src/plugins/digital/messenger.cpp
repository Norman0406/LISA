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

#include "messenger.h"
#include "ui_messenger.h"

#include <QDebug>
#include <QScrollBar>

using namespace Digital::Internal;

Messenger::Messenger(QWidget* parent)
    : QWidget(parent),
      m_textChanged(false)
{
    m_ui = new Ui::Messenger();
    m_ui->setupUi(this);

    // use a timer to trigger the text update
    m_updateTimer = new QTimer(this);
    m_updateTimer->setTimerType(Qt::PreciseTimer);
    m_updateTimer->setInterval(100);   // time in ms
    connect(m_updateTimer, &QTimer::timeout, this, &Messenger::updateText);
}

void Messenger::clear()
{
    m_message.clear();
    m_ui->txtReceived->clear();
    m_ui->txtSent->clear();
}

void Messenger::received(char character)
{
    if (m_ui->txtReceived->isEnabled()) {
        m_message += QChar::fromAscii(character);
        m_textChanged = true;
    }
}

void Messenger::updateText()
{
    if (m_textChanged) {
        m_ui->txtReceived->setText(m_message);
        QScrollBar* sb = m_ui->txtReceived->verticalScrollBar();
        sb->setValue(sb->maximum());

        m_textChanged = false;
    }
}

void Messenger::modemSelected(QString modemType)
{
    if (modemType.isEmpty()) {
        m_ui->txtReceived->setEnabled(false);
        m_ui->txtSent->setEnabled(false);
        m_updateTimer->stop();
    }
    else {
        m_ui->txtReceived->setEnabled(true);
        m_ui->txtSent->setEnabled(true);
        m_updateTimer->start();
    }
}
