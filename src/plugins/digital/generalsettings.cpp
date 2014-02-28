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

#include "generalsettings.h"
#include "digitalconstants.h"
#include "ui_generalsettings.h"

using namespace Digital::Internal;

GeneralSettings::GeneralSettings()
{
    setId(Digital::Constants::SETTINGS_ID_DIGITAL);
    setDisplayName(tr("General"));
    setCategory(Digital::Constants::SETTINGS_CATEGORY_DIGITAL);
    setDisplayCategory(QCoreApplication::translate("Digital",
        Digital::Constants::SETTINGS_TR_CATEGORY_DIGITAL));
    setCategoryIcon(QLatin1String(Digital::Constants::SETTINGS_CATEGORY_DIGITAL_ICON));
}

QWidget* GeneralSettings::createPage(QWidget *parent)
{
    m_page = new Ui::GeneralSettings();
    m_widget = new QWidget(parent);
    m_page->setupUi(m_widget);

    return m_widget;
}

void GeneralSettings::apply()
{
}

void GeneralSettings::finish()
{
}

bool GeneralSettings::matches(const QString&) const
{
    return false;
}
