#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace Hogra {

    class AudioDevice
    {
    public:

        void Init();

        ~AudioDevice();

        void Destroy();

        ALCdevice* Get() const;


    private:
        ALCdevice* device = nullptr;
    };
}

