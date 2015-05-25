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

#include "logbookmode.h"
#include "logbookwindow.h"
#include "logbookform.h"
#include "profiledata.h"

#include "callsignlookup/callsignlookupqrzcom.h"
#include "callsignlookup/callsigndata.h"

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/minisplitter.h>
#include <coreplugin/outputpane.h>
#include <coreplugin/settingsdatabase.h>
#include <coreplugin/icore.h>

using namespace Logbook::Internal;

LogbookMode::LogbookMode()
    : m_window(new LogbookWindow())
{
    setContext(Core::Context("Logbook.MainView"));
    setDisplayName(tr("Logbook"));
    setIcon(QIcon());
    setPriority(0);
    setId("Logbook.LogbookMode");
    setContextHelpId(QString());

    setWidget(m_window);

    m_logbookForm = new LogbookForm(0, this, m_window);
    ExtensionSystem::PluginManager::addObject(m_logbookForm);

    loadSettings();
}

LogbookMode::~LogbookMode()
{
    delete m_window;
    ExtensionSystem::PluginManager::removeObject(m_logbookForm);
    delete m_logbookForm;
}

void LogbookMode::loadSettings()
{
    QSettings* settings = Core::ICore::settings();

    settings->beginGroup(QLatin1String("Profiles"));
    int numProfiles = settings->value(QLatin1String("NumProfiles")).toInt();
    for (int i = 0; i < numProfiles; i++) {
        settings->beginGroup(QString(QLatin1String("Profile_%1")).arg(i));

        ProfileData newProfile(settings->value(QLatin1String("UUID")).toUuid(),
                               settings->value(QLatin1String("IsRemovable")).toBool(),
                               settings->value(QLatin1String("ProfileName")).toString());

        newProfile.setCallsign(settings->value(QLatin1String("Callsign")).toString());
        newProfile.setName(settings->value(QLatin1String("Name")).toString());
        newProfile.setStreet(settings->value(QLatin1String("Street")).toString());
        newProfile.setZipCode(settings->value(QLatin1String("ZipCode")).toString());
        newProfile.setCity(settings->value(QLatin1String("City")).toString());

        m_profiles.push_back(newProfile);

        settings->endGroup();
    }
    settings->endGroup();

    // create a default profile that can't be deleted and immediately save
    if (m_profiles.empty()) {
        ProfileData defaultProfile(ProfileData(false, tr("Default")));
        m_profiles.push_back(defaultProfile);
        saveSettings();
    }

    emit profilesChanged(m_profiles);
}

void LogbookMode::saveSettings()
{
    QSettings* settings = Core::ICore::settings();

    settings->beginGroup(QLatin1String("Profiles"));
    settings->setValue(QLatin1String("NumProfiles"), m_profiles.count());
    for (int i = 0; i < m_profiles.count(); i++) {
        settings->beginGroup(QString(QLatin1String("Profile_%1")).arg(i));
        const ProfileData& data = m_profiles[i];

        settings->setValue(QLatin1String("UUID"), data.getUuid());
        settings->setValue(QLatin1String("IsRemovable"), data.isRemovable());
        settings->setValue(QLatin1String("ProfileName"), data.getProfileName());
        settings->setValue(QLatin1String("Callsign"), data.getCallsign());
        settings->setValue(QLatin1String("Name"), data.getName());
        settings->setValue(QLatin1String("Street"), data.getStreet());
        settings->setValue(QLatin1String("ZipCode"), data.getZipCode());
        settings->setValue(QLatin1String("City"), data.getCity());

        settings->endGroup();
    }
    settings->endGroup();
}

LogbookForm* LogbookMode::getLogbookForm() const
{
    return m_logbookForm;
}

const QList<ProfileData>& LogbookMode::getProfiles() const
{
    return m_profiles;
}

void LogbookMode::setProfiles(const QList<ProfileData>& profiles)
{
    m_profiles = profiles;
    emit profilesChanged(m_profiles);
}

const ProfileData* LogbookMode::getProfile(QUuid uuid) const
{
    foreach (const ProfileData& data, m_profiles) {
        if (data.getUuid() == uuid)
            return &data;
    }
    return 0;
}
