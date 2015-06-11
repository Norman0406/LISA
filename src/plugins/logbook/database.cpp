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

#include "database.h"
#include <QDebug>
#include <QDateTime>
#include "logbookplugin.h"
#include "qsoentry.h"

using namespace Logbook::Internal;

Database::Database(QObject* parent)
   : QObject(parent),
     m_isOpen(false),
     m_model(0)
{
    m_database = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    m_database.setHostName(QLatin1String("localhost"));
}

Database::~Database()
{
    close();
}

bool Database::open(QString name)
{
    m_database.setDatabaseName(LogbookPlugin::resourcePath() + name + QLatin1String(".db"));

    // open database
    m_isOpen = m_database.open();

    if (m_isOpen) {
        // create necessary tables if they do not exist
        if (!m_database.tables().contains(QLatin1String("logbook"))) {
            qDebug() << "Initialize database file for the first time";
            createTables();
        }

        m_model = new QSqlRelationalTableModel(this, m_database);
        m_model->setTable(QLatin1String("logbook"));
        m_model->select();

        // TODO: create a list of qsoentries for each row in the model
    }

    //QSqlQuery query(m_database);

    //QsoEntry newEntry(this);
    //newEntry.setProperty("CallsignFrom", "DM6LN");
    //newEntry.CallsigTo = "DM5RS";

    /*QsoEntry newEntry(this);
    newEntry.setId(0);
    newEntry.setCallsign(QLatin1String("DM5RS"));
    newEntry.setOperator(QLatin1String("DM6LN"));

    updateOrInsert(newEntry);*/

    return m_isOpen;
}

void Database::createTables()
{
    if (m_isOpen) {
        QSqlQuery query(m_database);

        // create table if it does not already exist
        if (!query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS logbook ("
                                                  "ID INTEGER PRIMARY KEY,"
                                                  "Time INTEGER,"
                                                  "CallsignFrom VARCHAR(32),"
                                                  "CallsignTo VARCHAR(32),"
                                                  "Name VARCHAR(32),"
                                                  "Mode VARCHAR(32),"
                                                  "Frequency INTEGER,"
                                                  "Band INTEGER,"
                                                  "RstSend INTEGER,"
                                                  "RstRcvd INTEGER,"
                                                  "Country INTEGER,"
                                                  "Comment VARCHAR(50)"
                                                  ")"))) {
            qWarning() << "Could not create table";
        }
    }
}

QsoEntry* Database::getEntry(int index)
{
    // UNDONE
    if (m_isOpen) {
    }

    return 0;
}

void Database::updateOrInsert(const QsoEntry& entry)
{
    if (m_isOpen) {
        const QSqlRecord& record = entry.getRecord();

        QSqlTableModel model(this, m_database);
        model.setTable(QLatin1String("logbook"));

        // first, check if this entry already exists
    }
}

void Database::remove(const QsoEntry& entry)
{
    if (m_isOpen) {
    }
}

QAbstractTableModel* Database::getModel() const
{
    if (m_isOpen) {
        return m_model;
    }

    return 0;
}

void Database::close()
{
    m_model->submitAll();   // necessary?
    m_database.close();
    delete m_model;
    m_model = 0;
    foreach (QsoEntry* entry, m_entries)
        delete entry;
    m_entries.clear();
    m_isOpen = false;
}
