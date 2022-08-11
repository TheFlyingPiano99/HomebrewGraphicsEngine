#include "AudioListener.h"

namespace Hogra {

    std::vector<AudioListener*> AudioListener::heapAllocatedInstances = std::vector<AudioListener*>();

    AudioListener* AudioListener::Instantiate()
    {
        auto* instance = new AudioListener();
        heapAllocatedInstances.push_back(instance);
        return instance;
    }
    void AudioListener::Deallocate(AudioListener* toDelete)
    {
        auto iter = std::ranges::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), toDelete);
        if (heapAllocatedInstances.end() != iter) {
            heapAllocatedInstances.erase(iter);
            delete toDelete;
        }
    }
    void AudioListener::DeallocateAll()
    {
        for (auto instance : heapAllocatedInstances) {
            delete instance;
        }
        heapAllocatedInstances.clear();
    }
    
    void AudioListener::Init() {
        //TODO
    }
}