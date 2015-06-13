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

#ifndef QSOENTRY_H
#define QSOENTRY_H

#include <QObject>
#include <QDateTime>
#include <QtSql>
#include <QStandardItemModel>

namespace Logbook {
namespace Internal {

class QsoEntry
        : public QObject
{
    Q_OBJECT

    Q_ENUMS(Mode)
    Q_ENUMS(FrequencyBand)

    Q_PROPERTY(int ID MEMBER m_id)
    Q_PROPERTY(QDateTime DateTime MEMBER m_dateTime)
    Q_PROPERTY(QString Operator MEMBER m_operator)
    Q_PROPERTY(QString Callsign MEMBER m_callsign)
    Q_PROPERTY(QString Name MEMBER m_name)
    Q_PROPERTY(int Frequency MEMBER m_frequency)
    Q_PROPERTY(FrequencyBand FrequencyBand READ getBand WRITE setFrequencyByBand STORED false)
    Q_PROPERTY(Mode Mode MEMBER m_mode)
    Q_PROPERTY(int RstSent MEMBER m_rstSent)
    Q_PROPERTY(int RstSentNr MEMBER m_rstSentNr)
    Q_PROPERTY(int RstRcvd MEMBER m_rstRcvd)
    Q_PROPERTY(int RstRcvdNr MEMBER m_rstRcvdNr)
    Q_PROPERTY(QString Comment MEMBER m_comment)

public:
    QsoEntry(QObject *parent = 0);
    QsoEntry(const QSqlRecord&, QObject *parent = 0);
    QsoEntry(const QsoEntry&);

    QSqlRecord getRecord() const;
    void fromRecord(const QSqlRecord&);

    enum FrequencyBand{
        BAND_UNKNOWN,
        BAND_160M,
        BAND_80M,
        BAND_40M,
        BAND_30M,
        BAND_20M,
        BAND_17M,
        BAND_15M,
        BAND_12M,
        BAND_10M,
        BAND_6M,
        BAND_2M,
        BAND_70CM,
        BAND_23CM,
        BAND_13CM,
        // UNDONE
    };

    enum Mode
    {
        MODE_AM,
        MODE_FM,
        MODE_SSB,
        MODE_USB,
        MODE_LSB,
        MODE_PSK,
        MODE_RTTY,
        // UNDONE
    };

    QAbstractItemModel* getModel();

    void setId(int id);
    void setDateTime(QDateTime);
    void setOperator(QString);
    void setCallsign(QString);
    void setName(QString);
    void setFrequency(double);
    void setFrequencyByBand(FrequencyBand);
    void setRstSent(int);
    void setRstSentNr(int);
    void setRstRcvd(int);
    void setRstRcvdNr(int);
    void setComment(QString);

    int getId() const;
    const QDateTime& getDateTime() const;
    const QString& getCallsign() const;
    const QString& getOperator() const;
    const QString& getName() const;
    const double& getFrequency() const;
    FrequencyBand getBand() const;
    static FrequencyBand getBandByString(QString band);
    int getRstSent() const;
    int getRstSentNr() const;
    int getRstRcvd() const;
    int getRstRcvdNr() const;
    const QString& getComment() const;

private:
    void clear();
    bool inRange(const double& value, const double min, const double max) const;

    QStandardItemModel  m_model;
    int         m_id;
    QDateTime   m_dateTime;
    QString     m_operator;
    QString     m_callsign;
    QString     m_name;
    double      m_frequency; // MHz
    Mode        m_mode;
    int         m_rstSent;
    int         m_rstSentNr;
    int         m_rstRcvd;
    int         m_rstRcvdNr;
    QString     m_comment;
};

} // namespace Internal
} // namespace Logbook

#endif // QSOENTRY_H
