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

#ifndef LOGBOOKCONSTANTS_H
#define LOGBOOKCONSTANTS_H

namespace Logbook {
namespace Constants {

const char ICON_START_TX[]  = ":/digital/images/tx_start.png";
const char ICON_STOP_TX[]   = ":/digital/images/tx_stop.png";
const char ICON_CLEAR[]  = ":/digital/images/clear.png";

// Settings
const char SETTINGS_CATEGORY_LOGBOOK[] = "A.Logbook";
const char SETTINGS_CATEGORY_LOGBOOK_ICON[] = ":/core/images/category_core.png";
const char SETTINGS_TR_CATEGORY_LOGBOOK[] = QT_TRANSLATE_NOOP("Logbook", "Logbook");
const char SETTINGS_ID_LOGBOOK[] = "A.Logbook.General";
const char SETTINGS_ID_LOGBOOK_CALLSIGN_LOOKUP[] = "A.Logbook.CallsignLookup";

} // namespace Constants
} // namespace Logbook

#endif // LOGBOOKCONSTANTS_H
