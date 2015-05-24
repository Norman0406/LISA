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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "icontext.h"
#include "icore.h"

#include <utils/appmainwindow.h>
#include <utils/fileutils.h>

#include <QMap>
#include <QColor>

QT_BEGIN_NAMESPACE
class QSettings;
class QPrinter;
class QToolButton;
QT_END_NAMESPACE

namespace Core {

class StatusBarWidget;
class MessageManager;
class ModeManager;
class SettingsDatabase;

namespace Internal {

class FancyTabWidget;
class GeneralSettings;
class ShortcutSettings;
class StatusBarManager;
class VersionDialog;
class WindowSupport;

class MainWindow : public Utils::AppMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    bool init(QString *errorMessage);
    void extensionsInitialized();
    void aboutToShutdown();

    IContext *contextObject(QWidget *widget);
    void addContextObject(IContext *context);
    void removeContextObject(IContext *context);

    inline SettingsDatabase *settingsDatabase() const { return m_settingsDatabase; }
    virtual QPrinter *printer() const;
    IContext * currentContextObject() const;
    QStatusBar *statusBar() const;

    void updateAdditionalContexts(const Context &remove, const Context &add);

    void setSuppressNavigationWidget(bool suppress);

    void setOverrideColor(const QColor &color);

public slots:
    void exit();

    bool showOptionsDialog(Id page = Id(), QWidget *parent = 0);

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void aboutQtCreator();
    void aboutPlugins();
    void updateFocusWidget(QWidget *old, QWidget *now);
    void destroyVersionDialog();
    void restoreWindowState();

private:
    void updateContextObject(const QList<IContext *> &context);
    void updateContext();

    void registerDefaultContainers();
    void registerDefaultActions();

    void readSettings();
    void writeSettings();

    ICore *m_coreImpl;
    Context m_additionalContexts;
    SettingsDatabase *m_settingsDatabase;
    mutable QPrinter *m_printer;
    WindowSupport *m_windowSupport;
    MessageManager *m_messageManager;
    StatusBarManager *m_statusBarManager;
    ModeManager *m_modeManager;
    FancyTabWidget *m_modeStack;
    StatusBarWidget *m_outputView;
    VersionDialog *m_versionDialog;

    QList<IContext *> m_activeContext;

    QMap<QWidget *, IContext *> m_contextWidgets;

    GeneralSettings *m_generalSettings;
    ShortcutSettings *m_shortcutSettings;

    // actions
    QAction *m_focusToEditor;
    QAction *m_exitAction;
    QAction *m_optionsAction;
    QAction *m_toggleModeSelectorAction;
    QAction *m_themeAction;

    QColor m_overrideColor;
};

} // namespace Internal
} // namespace Core

#endif // MAINWINDOW_H
