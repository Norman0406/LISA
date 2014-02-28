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

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QTimer>

#include "audio/audiodevicein.h"
#include "audio/audiodeviceout.h"

namespace Digital {
namespace Internal {

class AudioDeviceList;

class AudioManager
        : public QObject
{
    Q_OBJECT

public:
    AudioManager(QObject*);

    void selectInputDevice();
    void selectOutputDevice();

    AudioDeviceIn* getInputDevice();
    AudioDeviceOut* getOutputDevice();

signals:
    void inDeviceReady(AudioDeviceIn*);
    void outDeviceReady(AudioDeviceOut*);

private:
    AudioDeviceList* m_deviceList;
    AudioDeviceIn* m_deviceIn;
    AudioDeviceOut* m_deviceOut;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIOMANAGER_H
