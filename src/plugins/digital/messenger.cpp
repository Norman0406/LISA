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
      m_receivedChanged(false),
      m_sentChanged(false)
{
    m_ui = new Ui::Messenger();
    m_ui->setupUi(this);

    // use a timer to trigger the text update
    m_receivedTimer = new QTimer(this);
    m_receivedTimer->setTimerType(Qt::PreciseTimer);
    m_receivedTimer->setInterval(100);   // time in ms
    connect(m_receivedTimer, &QTimer::timeout, this, &Messenger::updateReceived);

    m_sentTimer = new QTimer(this);
    m_sentTimer->setTimerType(Qt::PreciseTimer);
    m_sentTimer->setInterval(100);   // time in ms
    connect(m_sentTimer, &QTimer::timeout, this, &Messenger::updateSent);
}

void Messenger::clear()
{
    m_sent.clear();
    m_received.clear();
    m_ui->txtReceived->clear();
    m_ui->txtSent->clear();
}

void Messenger::received(char character)
{
    if (m_ui->txtReceived->isEnabled()) {
        m_received+= QChar::fromLatin1(character);
        m_receivedChanged = true;
    }
}

void Messenger::sent(char character)
{
    if (m_ui->txtSent->isEnabled()) {
        m_sent += QChar::fromLatin1(character);
        m_sentChanged = true;
    }
}

void Messenger::updateReceived()
{
    if (m_receivedChanged) {
        m_ui->txtReceived->setText(m_received);
        QScrollBar* sb = m_ui->txtReceived->verticalScrollBar();
        sb->setValue(sb->maximum());

        m_receivedChanged = false;
    }
}

void Messenger::updateSent()
{
    if (m_sentChanged) {
        m_ui->txtSent->setText(m_sent);
        QScrollBar* sb = m_ui->txtSent->verticalScrollBar();
        sb->setValue(sb->maximum());

        m_sentChanged = false;
    }
}

void Messenger::modemSelected(QString modemType)
{
    if (modemType.isEmpty()) {
        m_ui->txtReceived->setEnabled(false);
        m_ui->txtSent->setEnabled(false);
        m_receivedTimer->stop();
        m_sentTimer->stop();
    }
    else {
        m_ui->txtReceived->setEnabled(true);
        m_ui->txtSent->setEnabled(true);
        m_receivedTimer->start();
        m_sentTimer->start();
    }
}
