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

#ifndef AUDIODEVICEOUT_H
#define AUDIODEVICEOUT_H

#include "AudioDevice.h"
#include "AudioRingBuffer.h"
#include <QAudioOutput>

namespace Digital {
namespace Internal {

class AudioDeviceOut
        : public AudioDevice
{
    Q_OBJECT

public:
    AudioDeviceOut(QObject* parent, QAudioDeviceInfo deviceInfo);
    ~AudioDeviceOut();

    void start();
    void stop();
    void setVolume(float);
    float getVolume() const;

    AudioRingBuffer* getBuffer() const;

private slots:
    void stateChanged(QAudio::State);

protected:
    void iInit(const QAudioDeviceInfo&);

private:
    QAudioOutput*       m_audioOutput;
    AudioRingBuffer*    m_audioBuffer;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIODEVICEOUT_H
