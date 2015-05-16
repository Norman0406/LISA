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

#include "logbookwindow.h"
#include "logbooktoolbar.h"

#include <QVBoxLayout>
#include <QHeaderView>

using namespace Logbook::Internal;

LogbookWindow::LogbookWindow(QWidget *parent)
   : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // add the toolbar on the top
    layout->addWidget(new LogbookToolBar(this));

    // create the table view
    m_logbookView = new QTableView(this);
    m_logbookView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_logbookView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_logbookView->setAlternatingRowColors(true);

    // create database
    m_database = QSqlDatabase::addDatabase(QString::fromLatin1("QSQLITE"));
    m_database.setHostName(QString::fromLatin1("localhost"));
    m_database.setDatabaseName(QString::fromLatin1("logbook.sql"));
    m_database.open();

    // create model
    m_model = new QSqlRelationalTableModel(this, m_database);
    m_model->setTable(QString::fromLatin1("logbook"));
    m_model->select();

    // create proxy model
    m_proxyModel = new LogbookProxyModel;
    m_proxyModel->setSourceModel(m_model);

    // set the view model
    //m_logbookView->setModel(m_model);
    m_logbookView->setModel(m_proxyModel);
    m_logbookView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    layout->addWidget(m_logbookView);
}

LogbookWindow::~LogbookWindow()
{
    m_model->submitAll();
    m_database.close();
}
