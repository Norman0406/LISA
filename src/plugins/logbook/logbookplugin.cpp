/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "logbookplugin.h"

#include "logbookmode.h"
#include "generalsettings.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/id.h>

#include <QMenu>

namespace Logbook {
namespace Internal {

LogbookPlugin::LogbookPlugin()
{
}

LogbookPlugin::~LogbookPlugin()
{
}

bool LogbookPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    // Create our own menu to place in the Tools menu
    Core::ActionContainer *logbookMenu =
            Core::ActionManager::createMenu("Logbook.MainMenu");
    QMenu *menu = logbookMenu->menu();
    menu->setTitle(tr("&Logbook"));
    menu->setEnabled(false);

    // Request the Tools menu and add the Hello World menu to it
    Core::ActionContainer *toolsBar =
            Core::ActionManager::actionContainer(Core::Constants::M_TOOLS);

    // add logbook menu before tools menu
    Core::ActionContainer* menuBar =
            Core::ActionManager::actionContainer(Core::Constants::MENU_BAR);
    menuBar->addMenu(toolsBar, logbookMenu);

    // Add a mode with a push button based on BaseMode. Like the BaseView,
    // it will unregister itself from the plugin manager when it is deleted.
    m_logbookMode = new LogbookMode;
    addAutoReleasedObject(m_logbookMode);

    // TODO: move to core plugin or use other ways to determine which mode should be selected when
    // first starting up.
    Core::ModeManager::activateMode(m_logbookMode->id());

    return true;
}

void LogbookPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new GeneralSettings(this));
}

} // namespace Internal
} // namespace Logbook
