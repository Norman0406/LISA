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

#include "coreplugin.h"
#include "mainwindow.h"
#include "modemanager.h"
#include "infobar.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>

#include <extensionsystem/pluginerroroverview.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/macroexpander.h>
#include <utils/savefile.h>
#include <utils/stringutils.h>
#include <utils/theme/theme.h>
#include <utils/theme/theme_p.h>

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QApplication>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

CorePlugin::CorePlugin()
  : m_mainWindow(0)
{
    qRegisterMetaType<Id>();
}

CorePlugin::~CorePlugin()
{
    delete m_mainWindow;
    setCreatorTheme(0);
}

static QString absoluteThemePath(const QString &themeName, bool userProvidedTheme)
{
    if (themeName.isEmpty())
        return themeName;
    QString res = QDir::fromNativeSeparators(themeName);
    QFileInfo fi(res);
    bool tryRawName = userProvidedTheme || fi.isAbsolute();
    // Try the given name
    if (tryRawName && fi.exists())
        return fi.absoluteFilePath();
    const QString suffix = QLatin1String("creatortheme");
    // Try name.creatortheme
    if (fi.suffix() != suffix) {
        res = themeName + QLatin1Char('.') + suffix;
        fi.setFile(res);
        if (tryRawName && fi.exists())
            return fi.absoluteFilePath();
    }
    if (fi.path().isEmpty())
        return QString(); // absolute/relative path, but not found
    // If only name was given, look it up in qtcreator/themes
    res.prepend(ICore::resourcePath() + QLatin1String("/themes/"));
    return QFileInfo::exists(res) ? res : QString();
}

void CorePlugin::parseArguments(const QStringList &arguments)
{
    const QString defaultTheme = QLatin1String("default");
    QString themeName = ICore::settings()->value(
                QLatin1String(Constants::SETTINGS_THEME), defaultTheme).toString();
    QColor overrideColor;
    bool presentationMode = false;
    bool userProvidedTheme = false;

    for (int i = 0; i < arguments.size(); ++i) {
        if (arguments.at(i) == QLatin1String("-color")) {
            const QString colorcode(arguments.at(i + 1));
            overrideColor = QColor(colorcode);
            i++; // skip the argument
        }
        if (arguments.at(i) == QLatin1String("-presentationMode"))
            presentationMode = true;
        if (arguments.at(i) == QLatin1String("-theme")) {
            themeName = arguments.at(i + 1);
            userProvidedTheme = true;
            i++;
        }
    }

    QString themeURI = absoluteThemePath(themeName, userProvidedTheme);
    if (themeURI.isEmpty()) {
        themeName = defaultTheme;
        themeURI = QStringLiteral("%1/themes/%2.creatortheme").arg(ICore::resourcePath()).arg(themeName);
        if (themeURI.isEmpty()) {
            qCritical("%s", qPrintable(QCoreApplication::translate("Application", "No valid theme \"%1\"")
                                       .arg(themeName)));
        }
    }

    QSettings themeSettings(themeURI, QSettings::IniFormat);
    Theme *theme = new Theme(themeName, qApp);
    theme->readSettings(themeSettings);
    if (theme->flag(Theme::ApplyThemePaletteGlobally))
        QApplication::setPalette(theme->palette());
    setCreatorTheme(theme);

    // defer creation of these widgets until here,
    // because they need a valid theme set
    m_mainWindow = new MainWindow;
    ActionManager::setPresentationModeEnabled(presentationMode);

    if (overrideColor.isValid())
        m_mainWindow->setOverrideColor(overrideColor);
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    new ActionManager(this);
    Theme::initialPalette(); // Initialize palette before setting it
    qsrand(QDateTime::currentDateTime().toTime_t());
    parseArguments(arguments);
    const bool success = m_mainWindow->init(errorMessage);
    if (success) {
        InfoBar::initializeGloballySuppressed();
    }

    // Make sure we respect the process's umask when creating new files
    SaveFile::initializeUmask();

    MacroExpander *expander = Utils::globalMacroExpander();
    expander->registerVariable("CurrentDate:ISO", tr("The current date (ISO)."),
                               []() { return QDate::currentDate().toString(Qt::ISODate); });
    expander->registerVariable("CurrentTime:ISO", tr("The current time (ISO)."),
                               []() { return QTime::currentTime().toString(Qt::ISODate); });
    expander->registerVariable("CurrentDate:RFC", tr("The current date (RFC2822)."),
                               []() { return QDate::currentDate().toString(Qt::RFC2822Date); });
    expander->registerVariable("CurrentTime:RFC", tr("The current time (RFC2822)."),
                               []() { return QTime::currentTime().toString(Qt::RFC2822Date); });
    expander->registerVariable("CurrentDate:Locale", tr("The current date (Locale)."),
                               []() { return QDate::currentDate().toString(Qt::DefaultLocaleShortDate); });
    expander->registerVariable("CurrentTime:Locale", tr("The current time (Locale)."),
                               []() { return QTime::currentTime().toString(Qt::DefaultLocaleShortDate); });
    expander->registerPrefix("CurrentDate:", tr("The current date (QDate formatstring)."),
                             [](const QString &fmt) { return QDate::currentDate().toString(fmt); });
    expander->registerPrefix("CurrentTime:", tr("The current time (QTime formatstring)."),
                             [](const QString &fmt) { return QTime::currentTime().toString(fmt); });

    return success;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
    if (ExtensionSystem::PluginManager::hasError()) {
        auto errorOverview = new ExtensionSystem::PluginErrorOverview(m_mainWindow);
        errorOverview->setAttribute(Qt::WA_DeleteOnClose);
        errorOverview->setModal(true);
        errorOverview->show();
    }
}

bool CorePlugin::delayedInitialize()
{
    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    m_mainWindow->aboutToShutdown();
    return SynchronousShutdown;
}
