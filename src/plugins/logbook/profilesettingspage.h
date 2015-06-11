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

#ifndef PROFILESETTINGSPAGE_H
#define PROFILESETTINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QPointer>
#include "profiledata.h"

class QSettings;

namespace Logbook {
namespace Internal {

class LogbookMode;
class ProfileNameValidator;

namespace Ui {
    class ProfileSettingsPage;
}

class ProfileSettingsPage
        : public Core::IOptionsPage
{
    Q_OBJECT

public:
    ProfileSettingsPage(LogbookMode *logbookPlugin);
    ~ProfileSettingsPage();

    bool matches(const QString& searchKeyWord) const;
    QWidget* widget();
    void apply();
    void finish();

private slots:
    void addProfile();
    void removeProfile();
    void selectedProfileChanged(int);
    void profileRenamed(const QString&);

private:
    void setData();

    ProfileData* m_currentProfile;
    QList<ProfileData> m_profiles;

    Ui::ProfileSettingsPage* m_page;
    QWidget* m_widget;
    QSettings* m_settings;
    LogbookMode* m_logbookMode;
    ProfileNameValidator* m_profileNameValidator;
};

} // namespace Internal
} // namespace Logbook

#endif // PROFILESETTINGSPAGE_H
