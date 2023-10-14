#pragma once

#include <vector>

#include <AL/al.h>
#include <AL/alc.h>
#include "AudioBuffer.h"
#include "glm/glm.hpp"

namespace Hogra {

    class AudioSource
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:

        void Init(AudioBuffer* _buffer);

        ~AudioSource();

        void Play();

        void Stop();

        void SetGain(float gain);

        void SetPitch(float gain);

        void SetPosition(const glm::vec3& position);

        void SetVelocity(const glm::vec3& velocity);

        void SetLoop(bool loop);

        bool IsPlaying();

    private:
        ALuint alID;
        AudioBuffer* buffer = nullptr;
    };
}