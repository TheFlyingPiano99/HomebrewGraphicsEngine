#pragma once
#include <vector>
#include "SceneAudioSource.h"

namespace Hogra {
	class AudioManager
	{
	public:

		void Init();

		~AudioManager();

		void StopAll();

		void AddSource(SceneAudioSource* source);

		void Update();

	private:
		std::vector<SceneAudioSource*> sceneAudioSources;
	};
}

