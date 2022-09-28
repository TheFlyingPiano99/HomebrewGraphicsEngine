#pragma once

#include <vector>

#include "openAL/al.h"
#include "openAL/alc.h"
#include "AudioBuffer.h"
#include "glm/glm.hpp"

namespace Hogra {

	class AudioSource
	{
	public:

		void Init(const AudioBuffer* _buffer);

		~AudioSource();

		void Play();

		void Stop();

		void SetGain(float gain);

		void SetPitch(float gain);

		void SetPosition(const glm::vec3& position);

		void SetVelocity(const glm::vec3& velocity);

		void SetLoop(bool loop);

		bool IsPlaying();

	private:
		ALuint ID;
		const AudioBuffer* buffer = nullptr;

	};
}