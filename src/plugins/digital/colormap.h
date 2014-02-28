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

#ifndef COLORMAP_H
#define COLORMAP_H

#include <QObject>
#include <QMap>
#include <QColor>

namespace Digital {
namespace Internal {

class ColormapVisitor;

class Colormap
        : public QObject
{
    Q_OBJECT

public:
    Colormap(int, QObject*);
    ~Colormap();

    void accept(ColormapVisitor&);
    void addColor(int, const QColor&);
    int getRange() const;
    QColor getColorValue(int);


private:
    void clear();
    void buildColorTable();

    QMap<int, QColor> m_colors;
    QColor* m_colorTable;
    int m_range;
};

} // namespace Internal
} // namespace Digital

#endif // COLORMAP_H
