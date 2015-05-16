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

#ifndef LOGBOOKPROXYMODEL_H
#define LOGBOOKPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QMap>

namespace Logbook {
namespace Internal {

class LogbookProxyModel
        : public QAbstractProxyModel
{
    Q_OBJECT

private:
    enum ProxyColumns {
        PC_DATE,
        PC_TIME,
        PC_CALLSIGN,
        PC_NAME,
        PC_MODE,
        PC_FREQUENCY,
        PC_BAND,
        PC_RSTSENT,
        PC_RSTRCVD,
        PC_COUNTRY,
        PC_COMMENT
    };

public:
    LogbookProxyModel(QObject *parent = 0);
    ~LogbookProxyModel();

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &proxyIndex, int role) const;

    QMap<QString, QString> m_columnMap; // maps the columns names from the database to display names
    QMap<QModelIndex, QModelIndex> m_sourceToProxyMap;
    QMap<QModelIndex, QModelIndex> m_proxyToSourceMap;
    //QList<QString> m_columns;

    QMap<int, QPair<ProxyColumns, QString> > m_columns;
    QMap<ProxyColumns, QString> m_columnDatabaseMap;
};

} // namespace Internal
} // namespace Logbook

#endif // LOGBOOKPROXYMODEL_H
