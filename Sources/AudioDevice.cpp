#include "AudioDevice.h"

#include "AudioUtils.h"
#include "AudioContext.h"

#include <iostream>


namespace Hogra {
	
	void AudioDevice::Init() {
		device = alcOpenDevice(nullptr);
		if (nullptr == device) {
			std::cerr << "AUDIO DEVICE ERROR: Failed to open default audio device!";
			return;
		}
	}

	AudioDevice::~AudioDevice()
	{
		Destroy();
	}

	void AudioDevice::Destroy()
	{
		if (nullptr != device) {
			alcCloseDevice(device);
			device = nullptr;
		}
	}

	ALCdevice* AudioDevice::Get() const
	{
		return device;
	}
}
