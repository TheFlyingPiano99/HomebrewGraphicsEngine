#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "AudioDevice.h"

namespace Hogra {
    class AudioContext
    {
    public:
        void Init();

        void Destroy();

        ~AudioContext();

        void MakeCurrent();

        static void MakeDefaultCurrent();

    private:
        ALCcontext* context = nullptr;
        AudioDevice device;
    };
}

