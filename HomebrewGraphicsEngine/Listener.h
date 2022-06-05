#pragma once
#include "openAL/al.h"
#include "openAL/alc.h"

class Listener
{
public:
	Listener() {
		const ALCchar* deviceName = "Audio device 01";
		auto* device = alcOpenDevice(deviceName);
		ALCenum error = alcGetError(device);

	}

private:

};

