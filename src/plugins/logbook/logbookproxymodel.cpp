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

#include "logbookproxymodel.h"

using namespace Logbook::Internal;

LogbookProxyModel::LogbookProxyModel(QObject *parent)
   : QAbstractProxyModel(parent)
{
    //  names of the displayed columns
    m_columns[0] = qMakePair(PC_DATE, tr("Date"));
    m_columns[1] = qMakePair(PC_TIME, tr("Time"));
    m_columns[2] = qMakePair(PC_CALLSIGN, tr("Callsign"));
    m_columns[3] = qMakePair(PC_NAME, tr("Name"));
    m_columns[4] = qMakePair(PC_MODE, tr("Mode"));
    m_columns[5] = qMakePair(PC_FREQUENCY, tr("Frequency"));
    m_columns[6] = qMakePair(PC_BAND, tr("Band"));
    m_columns[7] = qMakePair(PC_RSTSENT, tr("RST Sent"));
    m_columns[8] = qMakePair(PC_RSTRCVD, tr("RST Rcvd"));
    m_columns[9] = qMakePair(PC_COUNTRY, tr("Country"));
    m_columns[10] = qMakePair(PC_COMMENT, tr("Comment"));

    // map between column names and actual database entries
    m_columnDatabaseMap[PC_CALLSIGN] = QString::fromLatin1("CALLSIGN");
    m_columnDatabaseMap[PC_NAME] = QString::fromLatin1("NAME");
    m_columnDatabaseMap[PC_MODE] = QString::fromLatin1("MODE");
    m_columnDatabaseMap[PC_FREQUENCY] = QString::fromLatin1("FREQ");
    m_columnDatabaseMap[PC_RSTSENT] = QString::fromLatin1("RSTSENT");
    m_columnDatabaseMap[PC_RSTRCVD] = QString::fromLatin1("RSTRCVD");
    m_columnDatabaseMap[PC_COUNTRY] = QString::fromLatin1("COUNTRY");
    m_columnDatabaseMap[PC_COMMENT] = QString::fromLatin1("COMMENT");
}

LogbookProxyModel::~LogbookProxyModel()
{
}

QModelIndex LogbookProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return proxyIndex;
}

QModelIndex LogbookProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return sourceIndex;
}

QModelIndex LogbookProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    return sourceModel()->index(row, column, parent);
}

QModelIndex LogbookProxyModel::parent(const QModelIndex &child) const
{
    return sourceModel()->parent(child);
}

int LogbookProxyModel::rowCount(const QModelIndex &parent) const
{
    return sourceModel()->rowCount(parent);
}

int LogbookProxyModel::columnCount(const QModelIndex &parent) const
{
    //return sourceModel()->columnCount(parent);
    return m_columns.size();
}

QVariant LogbookProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        /*QString header = sourceModel()->headerData(section, orientation, role).toString();
        return header;*/

        // find the column name
        QMap<int, QPair<ProxyColumns, QString> >::const_iterator it = m_columns.find(section);
        if (it != m_columns.end())
            return it.value().second;

        return QVariant();
    }
    return QVariant();
}

QVariant LogbookProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    // compute the columns that are derived from other database entries
    QMap<int, QPair<ProxyColumns, QString> >::const_iterator it = m_columns.find(proxyIndex.column());
    if (it != m_columns.end()) {
        const ProxyColumns& columnType = it.value().first;

        switch (columnType) {
        case PC_DATE:
            break;
        case PC_TIME:
            break;
        case PC_BAND:
            break;
        }
    }

    return sourceModel()->data(proxyIndex, role);
}
