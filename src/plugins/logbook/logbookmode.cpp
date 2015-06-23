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
#include "logbookentrypane.h"
#include "profiledata.h"
#include "importexport/importerfactory.h"
#include "importexport/exporterfactory.h"

#include "callsignlookup/callsignlookupqrzcom.h"
#include "callsignlookup/callsigndata.h"
#include "callsignlookup/callsignlookupmanager.h"

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

    m_logbookEntryPane = new LogbookEntryPane(this, m_window);
    connect(m_window, &LogbookWindow::qsoSelected, m_logbookEntryPane, &LogbookEntryPane::rowSelected);
    connect(m_window, &LogbookWindow::modelChanged, m_logbookEntryPane, &LogbookEntryPane::setModel);
    ExtensionSystem::PluginManager::addObject(m_logbookEntryPane);

    m_callsignLookupManager = new CallsignLookupManager(this);
    m_callsignLookupManager->registerEntryPane(m_logbookEntryPane);
    QObject::connect(m_logbookEntryPane, &LogbookEntryPane::lookupCallsign, m_callsignLookupManager, &CallsignLookupManager::lookup);

    m_importerFactory = new ImporterFactory();
    m_exporterFactory = new ExporterFactory();

    loadSettings();

    m_window->open();
}

LogbookMode::~LogbookMode()
{
    ExtensionSystem::PluginManager::removeObject(m_logbookEntryPane);
    delete m_logbookEntryPane;
    delete m_window;
    delete m_callsignLookupManager;
    delete m_importerFactory;
    delete m_exporterFactory;
}

void LogbookMode::loadSettings()
{
    QSettings* settings = Core::ICore::settings();

    int numProfiles = settings->beginReadArray(QLatin1String("Profiles"));
    for (int i = 0; i < numProfiles; i++) {
        settings->setArrayIndex(i);

        ProfileData newProfile(settings->value(QLatin1String("IsRemovable")).toBool(),
                               settings->value(QLatin1String("ProfileName")).toString());

        newProfile.setCallsign(settings->value(QLatin1String("Callsign")).toString());
        newProfile.setName(settings->value(QLatin1String("Name")).toString());
        newProfile.setStreet(settings->value(QLatin1String("Street")).toString());
        newProfile.setZipCode(settings->value(QLatin1String("ZipCode")).toString());
        newProfile.setCity(settings->value(QLatin1String("City")).toString());

        m_profiles.push_back(newProfile);
    }
    settings->endArray();

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

    settings->beginWriteArray(QLatin1String("Profiles"), m_profiles.count());
    for (int i = 0; i < m_profiles.count(); i++) {
        const ProfileData& data = m_profiles[i];
        settings->setArrayIndex(i);
        settings->setValue(QLatin1String("IsRemovable"), data.isRemovable());
        settings->setValue(QLatin1String("ProfileName"), data.getProfileName());
        settings->setValue(QLatin1String("Callsign"), data.getCallsign());
        settings->setValue(QLatin1String("Name"), data.getName());
        settings->setValue(QLatin1String("Street"), data.getStreet());
        settings->setValue(QLatin1String("ZipCode"), data.getZipCode());
        settings->setValue(QLatin1String("City"), data.getCity());
    }
    settings->endArray();
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

CallsignLookupManager* LogbookMode::getCallsignLookupManager()
{
    return m_callsignLookupManager;
}

void LogbookMode::addMenus(QMenu* mainMenu) const
{
    // add importer menus
    QMenu* importMenu = new QMenu(tr("&Import"), mainMenu);
    mainMenu->addMenu(importMenu);

    QStringList importTypes = m_importerFactory->getTypes();
    importMenu->setEnabled(!importTypes.empty());
    foreach (QString type, importTypes) {
        QAction* action = importMenu->addAction(type);
        action->setProperty("Type", type);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(importQsos()));
    }

    // add exporter menus
    QMenu* exportMenu = new QMenu(tr("&Export"), mainMenu);
    mainMenu->addMenu(exportMenu);

    QStringList exportTypes = m_exporterFactory->getTypes();
    exportMenu->setEnabled(!exportTypes.empty());
    foreach (QString type, exportTypes) {
        QAction* action = exportMenu->addAction(type);
        action->setProperty("Type", type);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(exportQsos()));
    }
}

void LogbookMode::importQsos()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString type = action->property("Type").toString();
    Importer* importer = m_importerFactory->createByType(type, this);
    if (importer) {
        qDebug() << "import";
        delete importer;
    }
    else
        qWarning() << "could create importer from type " << type;
}

void LogbookMode::exportQsos()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString type = action->property("Type").toString();
    Exporter* exporter = m_exporterFactory->createByType(type, this);
    if (exporter) {
        QModelIndexList selection = m_window->getExportQsos();
        exporter->doExport(selection);
        delete exporter;
    }
    else
        qWarning() << "could not create exporter from type " << type;
}
