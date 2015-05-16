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

#include "digitalplugin.h"
#include "digitalmode.h"
#include "generalsettings.h"
#include "soundsettings.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/id.h>

#include <QMenu>

namespace Digital {
namespace Internal {

DigitalPlugin::DigitalPlugin()
{
}

DigitalPlugin::~DigitalPlugin()
{
}

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

void DigitalPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new GeneralSettings());
    addAutoReleasedObject(new SoundSettings());
}

} // namespace Internal
} // namespace Digital
