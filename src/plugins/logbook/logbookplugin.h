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

#ifndef LOGBOOKPLUGIN_H
#define LOGBOOKPLUGIN_H

#include <extensionsystem/iplugin.h>

namespace Logbook {
namespace Internal {

class LogbookMode;

class LogbookPlugin
        : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Logbook.json")

public:
    LogbookPlugin();
    ~LogbookPlugin();

    bool initialize(const QStringList& arguments, QString* errorMessage);

    void extensionsInitialized();

private:
    LogbookMode* m_logbookMode;
};

} // namespace Internal
} // namespace Logbook

#endif // LOGBOOKPLUGIN_H
