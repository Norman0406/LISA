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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>

namespace Logbook {
namespace Internal {

class QsoEntry;

class Database
        : public QObject
{
    Q_OBJECT

public:
    Database(QObject *parent = 0);
    ~Database();

    bool open(QString);
    void close();
    void setName(QString);

    QSqlRelationalTableModel* getModel() const;
    QString getName() const;
    QString getFileName() const;
    bool isOpen() const;

signals:
    void modelChanged(QSqlRelationalTableModel*);

private:
    void createTables();

    QSqlDatabase m_database;
    QSqlRelationalTableModel* m_model;
    bool m_isOpen;
    QString m_fileName;
    QString m_name;
    const QString m_dbVersion;
};

} // namespace Internal
} // namespace Logbook

#endif // DATABASE_H
