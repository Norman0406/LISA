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

#include "logbookwindow.h"

#include "logbooktoolbar.h"

#include <QVBoxLayout>

using namespace Logbook::Internal;

LogbookWindow::LogbookWindow(QWidget *parent)
   : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // add the toolbar on the top
    layout->addWidget(new LogbookToolBar(this));

    // create a simple label
    QLabel* label = new QLabel(tr("Logbook"), this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}
