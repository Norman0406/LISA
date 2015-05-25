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

#ifndef LOGBOOKMODE_H
#define LOGBOOKMODE_H

#include <coreplugin/imode.h>

namespace Logbook {
namespace Internal {

class LogbookWindow;
class LogbookForm;
class CallsignLookup;
class ProfileData;

class LogbookMode
        : public Core::IMode
{
public:
    LogbookMode();
    ~LogbookMode();

    LogbookForm* getLogbookForm() const;
    const QList<ProfileData>& getProfiles() const;
    void setProfiles(const QList<ProfileData>&);

    void loadSettings();
    void saveSettings();

private:
    LogbookWindow* m_window;
    LogbookForm* m_logbookForm;
    CallsignLookup* m_callsignLookup;
    QList<ProfileData> m_profiles;
};


} // namespace Internal
} // namespace Logbook

#endif // LOGBOOKMODE_H
