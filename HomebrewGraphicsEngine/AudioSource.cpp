#include "AudioSource.h"

#include "AudioFile.h"


namespace Hogra {

    std::vector<AudioSource*> AudioSource::heapAllocatedInstances = std::vector<AudioSource*>();

    AudioSource* AudioSource::Instantiate()
    {
        auto* instance = new AudioSource();
        heapAllocatedInstances.push_back(instance);
        return instance;
    }
    void AudioSource::Deallocate(AudioSource* toDelete)
    {
        auto iter = std::ranges::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), toDelete);
        if (heapAllocatedInstances.end() != iter) {
            heapAllocatedInstances.erase(iter);
            delete toDelete;
        }
    }
    void AudioSource::DeallocateAll()
    {
        for (auto instance : heapAllocatedInstances) {
            delete instance;
        }
        heapAllocatedInstances.clear();
    }
    
    void AudioSource::Init(const AudioBuffer* _buffer) {
        this->buffer = _buffer;

        alGenSources(1, &ID);
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