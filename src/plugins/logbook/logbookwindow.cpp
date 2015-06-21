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
#include <QItemSelection>
#include <QMessageBox>
#include <QKeyEvent>
#include "database.h"

using namespace Logbook::Internal;

LogbookWindow::LogbookWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    m_deleteMessageBox = new QMessageBox();
    m_deleteMessageBox->setText(tr("Confirm delete"));
    m_deleteMessageBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    m_deleteMessageBox->setIcon(QMessageBox::Warning);
    m_deleteMessageBox->setButtonText(QMessageBox::Ok, tr("Delete items"));

    // add the toolbar on the top
    layout->addWidget(new LogbookToolBar(this));

    // create the table view
    m_logbookView = new QTableView(this);
    m_logbookView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_logbookView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_logbookView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_logbookView->installEventFilter(this);
    m_logbookView->setSortingEnabled(true);
    m_logbookView->setAlternatingRowColors(true);

    m_database = new Database(this);
    connect(m_database, &Database::modelChanged, this, &LogbookWindow::modelChanged);

    // create proxy model
    //m_proxyModel = new LogbookProxyModel;
    //m_proxyModel->setSourceModel(m_model);

    // TODO: add proxy model to filter the database by different views

    layout->addWidget(m_logbookView);

    connect(m_logbookView, &QTableView::doubleClicked, this, &LogbookWindow::rowSelected);
}

LogbookWindow::~LogbookWindow()
{
    close();
    delete m_database;
}

void LogbookWindow::open()
{
    m_database->close();
    m_database->setName(QLatin1String("DM6LN Logbook"));
    m_database->open(QLatin1String("logbook"));
    m_logbookView->setModel(m_database->getModel());

    // set the view model
    m_logbookView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    // hide the id column and set the row size to minimum
    m_logbookView->verticalHeader()->setDefaultSectionSize(m_logbookView->verticalHeader()->minimumSectionSize());
    m_logbookView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_logbookView->verticalHeader()->setSectionsClickable(false);
    m_logbookView->verticalHeader()->setHighlightSections(false);
    m_logbookView->horizontalHeader()->setHighlightSections(false);
    m_logbookView->setColumnHidden(0, true);
}

void LogbookWindow::close()
{
    m_database->close();
}

Database* LogbookWindow::getDatabase()
{
    return m_database;
}

QModelIndexList LogbookWindow::getExportQsos()
{
    QItemSelectionModel* selectionModel = m_logbookView->selectionModel();

    // import / export only the selected qsos
    QModelIndexList list = selectionModel->selectedRows();

    // import / export all qsos if no rows are selected
    if (list.count() == 0) {
        for (int i = 0; i < selectionModel->model()->rowCount(); i++) {
            QModelIndex index = selectionModel->model()->index(i, 0);
            list.push_back(index);
        }
    }

    return list;
}

void LogbookWindow::rowSelected(const QModelIndex& index)
{
    emit qsoSelected(index.row());
}

bool LogbookWindow::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent && keyEvent->key() == Qt::Key_Delete) {
            // UNDONE
            return true;
        }
    }

    return QWidget::eventFilter(target, event);
}

QTableView* LogbookWindow::getLogbookView()
{
    return m_logbookView;
}
