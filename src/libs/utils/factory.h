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

#ifndef FACTORY_H
#define FACTORY_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace Utils {

template <class T>
class Factory
{
public:
    ~Factory() {}

    T* createByType(QString type, QObject* parent)
    {
        T* newInstance = iCreate(type, parent);
        if (!newInstance) {
            qDebug() << QLatin1String("could not create instance: ") + type;
            return 0;
        }

        return newInstance;
    }

    QStringList getTypes()
    {
        return iGetTypes();
    }


protected:
    Factory() {}

    virtual QStringList iGetTypes() const = 0;
    virtual T* iCreate(QString, QObject*) const = 0;
};

} // namespace Utils

#endif // FACTORY_H
