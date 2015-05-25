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

#include "qsoentry.h"

using namespace Logbook::Internal;

QsoEntry::QsoEntry(QObject* parent)
   : QObject(parent)
{
    m_time = QDateTime::currentDateTimeUtc();
    m_frequency = 0;
    m_mode = MODE_SSB;
    m_rstSent = 59;
    m_rstRcvd = 59;
}

QSqlRecord QsoEntry::getRecord() const
{
    QSqlRecord record;

    // create a field for each property
    const QMetaObject* obj = metaObject();
    for (int i = 0; i < obj->propertyCount(); i++) {
        QMetaProperty metaProperty = obj->property(i);

        const char* name = metaProperty.name();

        // create field
        QSqlField field;
        field.setName(QLatin1String(name));
        field.setValue(property(name));

        // insert field into record
        record.insert(i, field);
    }

    return record;
}

bool QsoEntry::inRange(const double& value, const double min, const double max) const
{
    return value >= min && value <= max;
}

void QsoEntry::setId(int id)
{
    m_id = id;
}

void QsoEntry::setTime(QDateTime time)
{
    m_time = time;
}

void QsoEntry::setCallsign(QString callsign)
{
    m_callsign = callsign;
}

void QsoEntry::setOperator(QString op)
{
   m_operator = op;
}

void QsoEntry::setFrequency(double frequency)
{
    m_frequency = frequency;
}

void QsoEntry::setFrequencyByBand(FrequencyBand band)
{
    switch (band) {
    case BAND_160M:
        m_frequency = 1.8;
        break;
    case BAND_80M:
        m_frequency = 3.6;
        break;
    case BAND_40M:
        m_frequency = 7;
        break;
    case BAND_30M:
        m_frequency = 10.1;
        break;
    case BAND_20M:
        m_frequency = 14;
        break;
    case BAND_17M:
        m_frequency = 18.086;
        break;
    case BAND_15M:
        m_frequency = 21;
        break;
    case BAND_12M:
        m_frequency = 24.89;
        break;
    case BAND_10M:
        m_frequency = 28;
        break;
    case BAND_6M:
        m_frequency = 50.08;
        break;
    case BAND_2M:
        m_frequency = 144;
        break;
    case BAND_70CM:
        m_frequency = 433;
        break;
    case BAND_23CM:
        m_frequency = 1240;
        break;
    case BAND_13CM:
        m_frequency = 2320;
        break;
    case BAND_UNKNOWN:
    default:
        m_frequency = 0;
        break;
    }
}

void QsoEntry::setRstSend(int rstSent)
{
    m_rstSent = rstSent;
}

void QsoEntry::setRstRcvd(int rstRcvd)
{
    m_rstRcvd = rstRcvd;
}

void QsoEntry::setComment(QString comment)
{
    m_comment = comment;
}

int QsoEntry::getId() const
{
    return m_id;
}

const QDateTime& QsoEntry::getTime() const
{
    return m_time;
}

const QString& QsoEntry::getCallsign() const
{
    return m_callsign;
}

const QString& QsoEntry::getOperator() const
{
    return m_operator;
}

const double& QsoEntry::getFrequency() const
{
    return m_frequency;
}

QsoEntry::FrequencyBand QsoEntry::getBand() const
{
    if (inRange(m_frequency, 1.8, 2.0))
        return BAND_160M;
    else if (inRange(m_frequency, 3.5, 3.8))
        return BAND_80M;
    else if (inRange(m_frequency, 7, 7.2))
        return BAND_40M;
    else if (inRange(m_frequency, 10.1, 10.15))
        return BAND_30M;
    else if (inRange(m_frequency, 14, 14.35))
        return BAND_20M;
    else if (inRange(m_frequency, 18.068, 18.168))
        return BAND_17M;
    else if (inRange(m_frequency, 21, 21.45))
        return BAND_15M;
    else if (inRange(m_frequency, 24.89, 24.99))
        return BAND_12M;
    else if (inRange(m_frequency, 28, 29.7))
        return BAND_10M;
    else if (inRange(m_frequency, 50.08, 51))
        return BAND_6M;
    else if (inRange(m_frequency, 144, 146))
        return BAND_2M;
    else if (inRange(m_frequency, 430, 440))
        return BAND_70CM;
    else if (inRange(m_frequency, 1240, 1300))
        return BAND_23CM;
    else if (inRange(m_frequency, 2320, 2450))
        return BAND_13CM;
    else
        return BAND_UNKNOWN;
}

QsoEntry::FrequencyBand QsoEntry::getBandByString(QString band)
{
    if(band == QLatin1String("160M"))
        return BAND_160M;
    else if(band == QLatin1String("80M"))
        return BAND_80M;
    else if(band ==  QLatin1String("40M"))
        return BAND_40M;
    else if(band == QLatin1String("30M"))
        return BAND_30M;
    else if(band == QLatin1String("20M"))
        return BAND_20M;
    else if(band == QLatin1String("17M"))
        return BAND_17M;
    else if(band == QLatin1String("15M"))
        return BAND_15M;
    else if(band == QLatin1String("12M"))
        return BAND_12M;
    else if(band == QLatin1String("10M"))
        return BAND_10M;
    else if(band == QLatin1String("6M"))
        return BAND_6M;
    else if(band == QLatin1String("2M"))
        return BAND_2M;
    else if(band == QLatin1String("70CM"))
        return BAND_70CM;
    else if(band == QLatin1String("23CM"))
        return BAND_23CM;
    else if(band == QLatin1String("13CM"))
        return BAND_13CM;
    else
        return BAND_UNKNOWN;
}

int QsoEntry::getRstSent() const
{
    return m_rstSent;
}

int QsoEntry::getRstRcvd() const
{
    return m_rstRcvd;
}

const QString& QsoEntry::getComment() const
{
    return m_comment;
}
