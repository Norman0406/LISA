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

#ifndef AUDIODEVICEIN_H
#define AUDIODEVICEIN_H

#include "audiodevice.h"
#include <QAudioInput>

namespace Digital {
namespace Internal {

class AudioConsumer;
class AudioConsumerList;

class AudioDeviceIn
        : public AudioDevice
{
    Q_OBJECT

public:
    AudioDeviceIn(QObject* parent, QAudioDeviceInfo deviceInfo);
    ~AudioDeviceIn();

    void start();
    void stop();
    void setVolume(float);
    float getVolume() const;

    void registerConsumer(AudioConsumer*);
    void unregisterConsumer(AudioConsumer*);

private slots:
    void stateChanged(QAudio::State);

protected:
    void iInit(const QAudioDeviceInfo&);

private:
    QAudioInput*            m_audioInput;
    AudioConsumerList*      m_consumerList;
};

} // namespace Internal
} // namespace Digital

#endif // AUDIODEVICEIN_H
