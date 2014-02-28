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

#ifndef COLORMAPVISITOR_H
#define COLORMAPVISITOR_H

namespace Digital {
namespace Internal {

class Colormap;

// TODO: Factory pattern

// ColormapVisitor
class ColormapVisitor
{
public:
    virtual void visit(Colormap&) = 0;
};

// ColormapBlue
class ColormapBlue
        : public ColormapVisitor
{
public:
    void visit(Colormap&);
};

// ColormapCuteSDR
class ColormapCuteSDR
        : public ColormapVisitor
{
public:
    void visit(Colormap&);
};

// ColormapFldigi
class ColormapFldigi
        : public ColormapVisitor
{
public:
    void visit(Colormap&);
};

} // namespace Internal
} // namespace Digital

#endif // COLORMAPVISITOR_H
