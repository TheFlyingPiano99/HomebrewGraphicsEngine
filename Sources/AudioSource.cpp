#include "AudioSource.h"

#include "AudioFile.h"


namespace Hogra {

    void AudioSource::Init(AudioBuffer* _buffer) {
        this->buffer = _buffer;
        alGenSources(1, &alID);
        buffer->AddSource(alID);
        alSourcei(alID, AL_BUFFER, buffer->Get());
    }

    void AudioSource::Play()
    {
        alSourcePlay(alID);
    }

    void AudioSource::Stop()
    {
        alSourceStop(alID);
    }

    AudioSource::~AudioSource()
    {
        alDeleteSources(1, &alID);
    }
    void AudioSource::SetGain(float gain)
    {
        alSourcef(alID, AL_GAIN, gain);
    }
    void AudioSource::SetPitch(float pitch)
    {
        alSourcef(alID, AL_PITCH, pitch);
    }
    void AudioSource::SetPosition(const glm::vec3& position)
    {
        alSource3f(alID, AL_POSITION, position.x, position.y, position.z);
    }
    void AudioSource::SetVelocity(const glm::vec3& velocity)
    {
        alSource3f(alID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
    void AudioSource::SetLoop(bool loop)
    {
        alSourcei(alID, AL_LOOPING, (loop)? AL_TRUE : AL_FALSE);
    }
    bool AudioSource::IsPlaying()
    {
        ALint state;
        alGetSourcei(alID, AL_SOURCE_STATE, &state);
        return AL_PLAYING == state;
    }
}