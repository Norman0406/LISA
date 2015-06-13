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
    m_database->setName(QLatin1String("DM6LN Logbook"));
    m_database->open(QLatin1String("logbook"));

    // create proxy model
    //m_proxyModel = new LogbookProxyModel;
    //m_proxyModel->setSourceModel(m_model);

    // TODO: add proxy model to filter the database by different views

    // set the view model
    m_logbookView->setModel(m_database->getModel());
    m_logbookView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    layout->addWidget(m_logbookView);

    // hide the id column and set the row size to minimum
    m_logbookView->verticalHeader()->setDefaultSectionSize(m_logbookView->verticalHeader()->minimumSectionSize());
    m_logbookView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_logbookView->horizontalHeader()->setHighlightSections(false);
    m_logbookView->setColumnHidden(0, true);

    connect(m_logbookView, &QTableView::doubleClicked, this, &LogbookWindow::qsoSelected);
}

LogbookWindow::~LogbookWindow()
{
    //m_model->submitAll();
    m_database->close();
    delete m_database;
}

void LogbookWindow::qsoSelected(const QModelIndex& index)
{
    const QsoEntry* entry = m_database->getEntry(index.row());
    if (entry) {
        //QsoEntry& bla = *entry;
    }
}

void LogbookWindow::deleteSelection()
{
    /*QItemSelection selection = m_logbookView->selectionModel()->selection();

    if(selection.size() < 2)
    {
        m_deleteMessageBox->setInformativeText(tr("Do you want to delete the selected item?"));
    }
    else
    {
        m_deleteMessageBox->setInformativeText(tr("Do you want to delete the selected items?"));
    }

    m_deleteMessageBox->setDefaultButton(QMessageBox::Cancel);
    int ret = m_deleteMessageBox->exec();

    if(ret == QMessageBox::Ok)
    {
        QList<int> rows;
        foreach( const QModelIndex & index, selection.indexes() ) {
            rows.append( index.row() );
        }
        qSort( rows );

        int prev = -1;
        for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
            int current = rows[i];
            if( current != prev ) {
                m_model->removeRows( current, 1 );
                prev = current;
            }
        }
        m_model->submit();
        m_model->select();
    }*/
}

bool LogbookWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent && keyEvent->key() == Qt::Key_Delete) {
            deleteSelection();
            return true;
        }
    }

    return QWidget::eventFilter(target, event);
}

QTableView* LogbookWindow::getLogbookView()
{
    return m_logbookView;
}

void LogbookWindow::addQso(QMap<QString, QString>* data) {
    /*qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate);
    int row = m_model->rowCount();
    m_model->insertRow(row);
    qDebug() << data->value(QLatin1String("Callsign"));
    m_model->setData(m_model->index(row,1), data->value(QLatin1String("Datetime")));
    m_model->setData(m_model->index(row,2), data->value(QLatin1String("CallsignFrom")));
    m_model->setData(m_model->index(row,3), data->value(QLatin1String("Callsign")));
    m_model->setData(m_model->index(row,4), data->value(QLatin1String("Name")));
    m_model->setData(m_model->index(row,5), data->value(QLatin1String("Mode")));
    m_model->setData(m_model->index(row,6), data->value(QLatin1String("Frequency")));
    m_model->setData(m_model->index(row,7), data->value(QLatin1String("Band")));
    m_model->setData(m_model->index(row,8), data->value(QLatin1String("RSTSend")));
    m_model->setData(m_model->index(row,9), data->value(QLatin1String("RSTRcvd")));

    m_model->submit();
    m_model->select();*/
}
