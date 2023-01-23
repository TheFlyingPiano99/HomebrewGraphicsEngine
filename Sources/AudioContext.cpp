#include "AudioContext.h"

#include <iostream>

void Hogra::AudioContext::Init()
{
	device.Init();

	context = alcCreateContext(device.Get(), nullptr);
	if (nullptr == context) {
		std::cerr << "AUDIO CONTEXT ERROR: Failed to create audio context!" << std::endl;
		return;
	}
}

void Hogra::AudioContext::Destroy()
{
	if (nullptr != context) {
		MakeDefaultCurrent();
		alcDestroyContext(context);
		context = nullptr;
	}
}

Hogra::AudioContext::~AudioContext()
{
	Destroy();
}

void Hogra::AudioContext::MakeCurrent()
{
	alcMakeContextCurrent(context);
}

void Hogra::AudioContext::MakeDefaultCurrent()
{
	alcMakeContextCurrent(nullptr);
}
