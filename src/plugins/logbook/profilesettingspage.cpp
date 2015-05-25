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

#include "profilesettingspage.h"
#include "logbookconstants.h"
#include "coreplugin/icore.h"
#include "logbookmode.h"
#include "ui_profilesettingspage.h"

#include <QSettings>

#include <QLabel>

using namespace Logbook::Internal;

ProfileSettingsPage::ProfileSettingsPage(LogbookMode* logbookMode)
    : m_logbookMode(logbookMode),
      m_currentProfile(0)
{
    setId(Logbook::Constants::SETTINGS_ID_LOGBOOK);
    setDisplayName(tr("Profiles"));
    setCategory(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK);
    setDisplayCategory(QCoreApplication::translate("Logbook",
        Logbook::Constants::SETTINGS_TR_CATEGORY_LOGBOOK));
    setCategoryIcon(QLatin1String(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK_ICON));
    m_settings = Core::ICore::settings();
}

bool ProfileSettingsPage::matches(const QString& searchKeyWord) const
{
    return false;
}

QWidget* ProfileSettingsPage::widget()
{
    if (!m_widget) {
        m_page = new Ui::ProfileSettingsPage();
        m_widget = new QWidget;
        m_page->setupUi(m_widget);

        const QList<ProfileData>& profiles = m_logbookMode->getProfiles();
        foreach (const ProfileData& data, profiles) {
            m_profiles.push_back(ProfileData(data));
            m_page->listWidgetProfiles->addItem(data.getProfileName());
        }

        connect(m_page->listWidgetProfiles, &QListWidget::currentRowChanged, this, &ProfileSettingsPage::selectedProfileChanged);
        connect(m_page->lineEditProfile, &QLineEdit::textChanged, this, &ProfileSettingsPage::profileRenamed);
        connect(m_page->pushButtonAdd, &QPushButton::clicked, this, &ProfileSettingsPage::addProfile);
        connect(m_page->pushButtonRemove, &QPushButton::clicked, this, &ProfileSettingsPage::removeProfile);

        m_page->listWidgetProfiles->setCurrentRow(0);
    }
    return m_widget;
}

void ProfileSettingsPage::apply()
{
    // update profile data and save settings
    setData();
    m_logbookMode->setProfiles(m_profiles);
    m_logbookMode->saveSettings();
}

void ProfileSettingsPage::finish()
{
    delete m_widget;
    delete m_page;
    m_page = 0;
    m_profiles.clear();
    m_currentProfile = 0;
}

void ProfileSettingsPage::selectedProfileChanged(int index)
{
    // before changing, update the currently selected profile with data from the form
    setData();

    // set new profile data
    ProfileData& data = m_profiles[index];
    m_currentProfile = &data;

    m_page->pushButtonRemove->setEnabled(data.isRemovable());

    m_page->lineEditProfile->setText(data.getProfileName());
    m_page->lineEditCallsign->setText(data.getCallsign());
    m_page->lineEditName->setText(data.getName());
    m_page->lineEditStreet->setText(data.getStreet());
    m_page->lineEditZipCode->setText(data.getZipCode());
    m_page->lineEditCity->setText(data.getCity());
}

void ProfileSettingsPage::setData()
{
    if (m_currentProfile) {
        m_currentProfile->setProfileName(m_page->lineEditProfile->text());
        m_currentProfile->setName(m_page->lineEditName->text());
        m_currentProfile->setCallsign(m_page->lineEditCallsign->text());
        m_currentProfile->setStreet(m_page->lineEditStreet->text());
        m_currentProfile->setZipCode(m_page->lineEditZipCode->text());
        m_currentProfile->setCity(m_page->lineEditCity->text());
    }
}

void ProfileSettingsPage::profileRenamed(const QString& name)
{
    m_page->listWidgetProfiles->currentItem()->setText(name);
}

void ProfileSettingsPage::addProfile()
{
    QString profileName = QString(QLatin1String("Profile %1")).arg(m_profiles.count() + 1);
    m_profiles.push_back(ProfileData(true, profileName));
    m_page->listWidgetProfiles->addItem(profileName);
    m_page->listWidgetProfiles->setCurrentRow(m_page->listWidgetProfiles->count() - 1);
}

void ProfileSettingsPage::removeProfile()
{
    int index = m_page->listWidgetProfiles->currentRow();
    bool atEnd = index == m_page->listWidgetProfiles->count() - 1;

    // remove item
    m_page->listWidgetProfiles->takeItem(index);
    m_profiles.removeAt(index);

    // select next entry
    if (atEnd) {
        // if at end, select last entry
        m_page->listWidgetProfiles->setCurrentRow(m_page->listWidgetProfiles->count() - 1);
    }
    else {
        // else, select previous entry
        m_page->listWidgetProfiles->setCurrentRow(index);
    }
}
