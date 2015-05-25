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

#include "generalsettings.h"
#include "logbookconstants.h"
#include "coreplugin/icore.h"
#include "logbookplugin.h"
#include "ui_generalsettings.h"

#include <QSettings>

using namespace Logbook::Internal;

GeneralSettings::GeneralSettings(LogbookPlugin* logbookPlugin) : m_logbookPlugin(logbookPlugin)
{
    setId(Logbook::Constants::SETTINGS_ID_LOGBOOK);
    setDisplayName(tr("General"));
    setCategory(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK);
    setDisplayCategory(QCoreApplication::translate("Logbook",
        Logbook::Constants::SETTINGS_TR_CATEGORY_LOGBOOK));
    setCategoryIcon(QLatin1String(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK_ICON));
    m_settings = Core::ICore::settings();
}

bool GeneralSettings::matches(const QString& searchKeyWord) const
{
    return false;
}

QWidget* GeneralSettings::widget()
{
    if (!m_widget) {
        m_page = new Ui::GeneralSettings();
        m_widget = new QWidget;
        m_page->setupUi(m_widget);
        m_page->lineEditProfil->setText(m_settings->value(QString::fromLatin1("logbook/profil")).toString());
        m_page->lineEditCallsign->setText(m_settings->value(QString::fromLatin1("logbook/callsign")).toString());
        m_page->lineEditStreet->setText(m_settings->value(QString::fromLatin1("logbook/street")).toString());
        m_page->lineEditZipCode->setText(m_settings->value(QString::fromLatin1("logbook/zipcode")).toString());
        m_page->lineEditCity->setText(m_settings->value(QString::fromLatin1("logbook/city")).toString());
    }
    return m_widget;
}

void GeneralSettings::apply()
{
    m_settings->setValue(QString::fromLatin1("logbook/profil"), m_page->lineEditProfil->text());
    m_settings->setValue(QString::fromLatin1("logbook/callsign"), m_page->lineEditCallsign->text());
    m_settings->setValue(QString::fromLatin1("logbook/street"), m_page->lineEditStreet->text());
    m_settings->setValue(QString::fromLatin1("logbook/zipcode"), m_page->lineEditZipCode->text());
    m_settings->setValue(QString::fromLatin1("logbook/city"), m_page->lineEditCity->text());

}

void GeneralSettings::finish()
{
    delete m_widget;
    delete m_page;
    m_page = 0;
}
