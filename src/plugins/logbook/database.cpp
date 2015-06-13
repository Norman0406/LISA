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
#include <app/app_version.h>
#include "logbookplugin.h"
#include "qsoentry.h"

using namespace Logbook::Internal;

Database::Database(QObject* parent)
   : QObject(parent),
     m_isOpen(false),
     m_model(0),
     m_dbVersion(QLatin1String("1.0.0"))
{
    m_database = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    m_database.setHostName(QLatin1String("localhost"));
}

Database::~Database()
{
    close();
}

bool Database::open(QString fileName)
{
    if (m_isOpen)
        return true;

    fileName = fileName;
    m_database.setDatabaseName(LogbookPlugin::resourcePath() + fileName + QLatin1String(".db"));

    // open database
    m_isOpen = m_database.open();

    if (m_isOpen) {
        // create necessary tables if they do not exist
        if (!m_database.tables().contains(QLatin1String("logbook"))) {
            qDebug() << "Initialize database file for the first time";
            createTables();
        }

        QSqlQuery query(m_database);
        if (!query.exec(QLatin1String("SELECT * from info"))) {
            qWarning() << "could not get database info";
            close();
        }
        else {
            // get database information
            query.next();
            QSqlRecord infoRec = query.record();
            m_name = infoRec.value(QLatin1String("Name")).toString();
            QString lisaVersion = infoRec.value(QLatin1String("LISAVersion")).toString();
            qDebug() << "database was created with LISA version " << lisaVersion;

            QString dbVersion = infoRec.value(QLatin1String("DBVersion")).toString();
            if (dbVersion != m_dbVersion)
                qWarning() << "incompatible database versions";

            m_model = new QSqlRelationalTableModel(this, m_database);
            m_model->setTable(QLatin1String("logbook"));
            m_model->select();
        }
    }

    return m_isOpen;
}

void Database::createTables()
{
    if (m_isOpen) {
        QSqlQuery query(m_database);

        // create info table
        if (!query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS info ("
                                                  "Name VARCHAR(64),"
                                                  "DBVersion VARCHAR(8),"
                                                  "LISAVersion VARCHAR(8)"
                                                  ")"))) {
            qWarning() << "could not create info table";
        }

        if (m_name.isEmpty())
            m_name = m_fileName;

        // insert data into info table
        query.prepare(QLatin1String("INSERT INTO info (Name, DBVersion, LISAVersion) "
                                    "VALUES (:name, :dbversion, :lisaversion)"));
        query.bindValue(QLatin1String(":name"), m_name);
        query.bindValue(QLatin1String(":dbversion"), m_dbVersion);
        query.bindValue(QLatin1String(":lisaversion"), QLatin1String(Core::Constants::LISA_VERSION_LONG));
        if (!query.exec())
            qWarning() << "could not insert values into info table";

        // create table if it does not already exist
        if (!query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS logbook ("
                                                  "ID INTEGER PRIMARY KEY,"
                                                  "DateTime INTEGER,"
                                                  "Operator VARCHAR(32),"
                                                  "Callsign VARCHAR(32),"
                                                  "Name VARCHAR(32),"
                                                  "Frequency INTEGER,"
                                                  "Mode VARCHAR(32),"
                                                  "RstSent INTEGER,"
                                                  "RstSentNr INTEGER,"
                                                  "RstRcvd INTEGER,"
                                                  "RstRcvdNr INTEGER,"
                                                  "Comment VARCHAR(1024)"
                                                  ")"))) {
            qWarning() << "could not create logbook table";
        }

        // create custom fields table

        // create profiles table

        // create stations table
    }
}

QSqlRelationalTableModel* Database::getModel() const
{
    if (m_isOpen)
        return m_model;

    return 0;
}

QString Database::getName() const
{
    return m_name;
}

QString Database::getFileName() const
{
    return m_fileName;
}

bool Database::isOpen() const
{
    return m_isOpen;
}

void Database::setName(QString name)
{
    if (!m_isOpen)
        m_name = name;
}

void Database::close()
{
    if (m_isOpen) {
        // close database
        m_model->submitAll();   // necessary?
        m_database.close();

        m_isOpen = false;
        m_fileName.clear();

        // free memory
        delete m_model;
        m_model = 0;
    }
}
