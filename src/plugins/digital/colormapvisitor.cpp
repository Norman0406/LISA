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

#include "colormapvisitor.h"
#include "colormap.h"

using namespace Digital::Internal;

// ColormapBlue
void ColormapBlue::visit(Colormap& map)
{
    int range = map.getRange();
    map.addColor(0, QColor(0, 0, 30));
    map.addColor((int)(range / 3.0), QColor(0, 0, 83));
    map.addColor(range - 1, QColor(255, 255, 255));
}

// ColormapCuteSDR
void ColormapCuteSDR::visit(Colormap& map)
{
    int range = map.getRange();
    map.addColor(0, QColor(0, 0, 0));
    map.addColor((int)((43 / 255.0) * range), QColor(0, 0, 255));
    map.addColor((int)((87 / 255.0) * range), QColor(0, 255, 255));
    map.addColor((int)((120 / 255.0) * range), QColor(0, 255, 0));
    map.addColor((int)((154 / 255.0) * range), QColor(255, 255, 0));
    map.addColor((int)((217 / 255.0) * range), QColor(255, 0, 0));
    map.addColor(range - 1, QColor(255, 0, 128));
}

// ColormapFldigi
void ColormapFldigi::visit(Colormap& map)
{
    int range = map.getRange();
    map.addColor(0, QColor(0, 0, 0));
    map.addColor((int)(0), QColor(0, 0, 0));
    map.addColor((int)((1 / 8.0) * range), QColor(0, 0, 136));
    map.addColor((int)((2 / 8.0) * range), QColor(0, 19, 198));
    map.addColor((int)((3 / 8.0) * range), QColor(0, 32, 239));
    map.addColor((int)((4 / 8.0) * range), QColor(172, 167, 105));
    map.addColor((int)((5 / 8.0) * range), QColor(194, 198, 49));
    map.addColor((int)((6 / 8.0) * range), QColor(225, 228, 107));
    map.addColor((int)((7 / 8.0) * range), QColor(255, 255, 0));
    map.addColor((int)(range - 1), QColor(251, 51, 0));
}
