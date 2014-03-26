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

#include "logbookplugin.h"

#include "logbookmode.h"
#include "generalsettings.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/id.h>

#include <QDebug>
#include <QtPlugin>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

using namespace Logbook::Internal;

LogbookPlugin::LogbookPlugin()
    : m_logbookMode(0)
{
}

LogbookPlugin::~LogbookPlugin()
{
}

bool LogbookPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    // create menu entries
    Core::ActionContainer* digitalMenu =
            Core::ActionManager::createMenu("Logbook.MainMenu");
    QMenu* menu = digitalMenu->menu();
    menu->setTitle(tr("&Logbook"));
    menu->setEnabled(false);

    Core::ActionContainer* toolsBar =
            Core::ActionManager::actionContainer(Core::Constants::M_TOOLS);

    // add digital menu before tools menu
    Core::ActionContainer* menuBar =
            Core::ActionManager::actionContainer(Core::Constants::MENU_BAR);
    menuBar->addMenu(toolsBar, digitalMenu);

    // add the mode to the mode selector
    m_logbookMode = new LogbookMode;
    addAutoReleasedObject(m_logbookMode);

    // TODO: move to core plugin or use other ways to determine which mode should be selected when
    // first starting up.
    Core::ModeManager::activateMode(m_logbookMode->id());

    return true;
}

void LogbookPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new GeneralSettings());
}

Q_EXPORT_PLUGIN(Logbook::Internal::LogbookPlugin)
