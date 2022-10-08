#include "AudioSource.h"

#include "AudioFile.h"


namespace Hogra {

    void AudioSource::Init(AudioBuffer* _buffer) {
        this->buffer = _buffer;
        alGenSources(1, &ID);
        buffer->AddSource(ID);
        alSourcei(ID, AL_BUFFER, buffer->Get());
    }

    void AudioSource::Play()
    {
        alSourcePlay(ID);
    }

    void AudioSource::Stop()
    {
        alSourceStop(ID);
    }

    AudioSource::~AudioSource()
    {
        alDeleteSources(1, &ID);
    }
    void AudioSource::SetGain(float gain)
    {
        alSourcef(ID, AL_GAIN, gain);
    }
    void AudioSource::SetPitch(float pitch)
    {
        alSourcef(ID, AL_PITCH, pitch);
    }
    void AudioSource::SetPosition(const glm::vec3& position)
    {
        alSource3f(ID, AL_POSITION, position.x, position.y, position.z);
    }
    void AudioSource::SetVelocity(const glm::vec3& velocity)
    {
        alSource3f(ID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
    void AudioSource::SetLoop(bool loop)
    {
        alSourcei(ID, AL_LOOPING, (loop)? AL_TRUE : AL_FALSE);
    }
    bool AudioSource::IsPlaying()
    {
        ALint state;
        alGetSourcei(ID, AL_SOURCE_STATE, &state);
        return AL_PLAYING == state;
    }
}