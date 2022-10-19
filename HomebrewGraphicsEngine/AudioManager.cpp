#include "AudioManager.h"
#include "HograTime.h"

void Hogra::AudioManager::Init()
{
}

Hogra::AudioManager::~AudioManager()
{
	StopAll();
}

void Hogra::AudioManager::StopAll()
{
	for (auto& source : sceneAudioSources) {
		source->Stop();
	}
}

void Hogra::AudioManager::AddSource(SceneAudioSource* source)
{
	if (sceneAudioSources.end() != std::ranges::find(sceneAudioSources.begin(), sceneAudioSources.end(), source)) {
		return;		// Already stored
	}
	sceneAudioSources.push_back(source);
}

void Hogra::AudioManager::Update()
{
	for (auto& source : sceneAudioSources) {
		source->LatePhysicsUpdate(0.0f);
	}
}
