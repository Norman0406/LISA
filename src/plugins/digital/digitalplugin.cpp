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

#include "digitalplugin.h"

#include "digitalmode.h"
#include "generalsettings.h"
#include "soundsettings.h"

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

using namespace Digital::Internal;

/*! Constructs the Hello World plugin. Normally plugins don't do anything in
    their constructor except for initializing their member variables. The
    actual work is done later, in the initialize() and extensionsInitialized()
    functions.
*/
DigitalPlugin::DigitalPlugin()
    : m_digitalMode(0)
{
}

/*! Plugins are responsible for deleting objects they created on the heap, and
    to unregister objects from the plugin manager that they registered there.
*/
DigitalPlugin::~DigitalPlugin()
{
}

/*! Initializes the plugin. Returns true on success.
    Plugins want to register objects with the plugin manager here.

    \a errorMessage can be used to pass an error message to the plugin system,
       if there was any.
*/
bool DigitalPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    // create menu entries
    Core::ActionContainer* digitalMenu =
            Core::ActionManager::createMenu("Digital.MainMenu");
    QMenu* menu = digitalMenu->menu();
    menu->setTitle(tr("&Digital"));
    menu->setEnabled(false);

    Core::ActionContainer* toolsBar =
            Core::ActionManager::actionContainer(Core::Constants::M_TOOLS);

    // add digital menu before tools menu
    Core::ActionContainer* menuBar =
            Core::ActionManager::actionContainer(Core::Constants::MENU_BAR);
    menuBar->addMenu(toolsBar, digitalMenu);

    // add the mode to the mode selector
    m_digitalMode = new DigitalMode;
    addAutoReleasedObject(m_digitalMode);

    // TODO: move to core plugin or use other ways to determine which mode should be selected when
    // first starting up.
    Core::ModeManager::activateMode(m_digitalMode->id());

    return true;
}

/*! Notification that all extensions that this plugin depends on have been
    initialized. The dependencies are defined in the plugins .pluginspec file.

    Normally this function is used for things that rely on other plugins to have
    added objects to the plugin manager, that implement interfaces that we're
    interested in. These objects can now be requested through the
    PluginManagerInterface.

    The HelloWorldPlugin doesn't need things from other plugins, so it does
    nothing here.
*/
void DigitalPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new GeneralSettings());
    addAutoReleasedObject(new SoundSettings());
}

Q_EXPORT_PLUGIN(Digital::Internal::DigitalPlugin)
