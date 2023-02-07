#include "AudioSource.h"

#include "AudioFile.h"


namespace Hogra {

    void AudioSource::Init(AudioBuffer* _buffer) {
        this->buffer = _buffer;
        alGenSources(1, &glID);
        buffer->AddSource(glID);
        alSourcei(glID, AL_BUFFER, buffer->Get());
    }

    void AudioSource::Play()
    {
        alSourcePlay(glID);
    }

    void AudioSource::Stop()
    {
        alSourceStop(glID);
    }

    AudioSource::~AudioSource()
    {
        alDeleteSources(1, &glID);
    }
    void AudioSource::SetGain(float gain)
    {
        alSourcef(glID, AL_GAIN, gain);
    }
    void AudioSource::SetPitch(float pitch)
    {
        alSourcef(glID, AL_PITCH, pitch);
    }
    void AudioSource::SetPosition(const glm::vec3& position)
    {
        alSource3f(glID, AL_POSITION, position.x, position.y, position.z);
    }
    void AudioSource::SetVelocity(const glm::vec3& velocity)
    {
        alSource3f(glID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
    void AudioSource::SetLoop(bool loop)
    {
        alSourcei(glID, AL_LOOPING, (loop)? AL_TRUE : AL_FALSE);
    }
    bool AudioSource::IsPlaying()
    {
        ALint state;
        alGetSourcei(glID, AL_SOURCE_STATE, &state);
        return AL_PLAYING == state;
    }
}