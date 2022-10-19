#pragma once
#include "Component.h"
#include "AudioSource.h"
#include "glm/glm.hpp"
#include "PositionProvider.h"

namespace Hogra {
	class SceneAudioSource : public Component
	{
	public:

		void Init(AudioSource* _source);

		void Play();

		void Stop();

		void SetPositionProvider(PositionProvider* provider);

		void SetGain(float g);
		
		void SetPitch(float p);

		void SetPosition(const glm::vec3& _position);

		void SetVelocity(const glm::vec3& _velocity);

		void SetLoop(bool _loop);

	
		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		bool IsPlaying() const {
			return isPlaying;
		}

	private:
		AudioSource* audioSource;

		PositionProvider* positionProvider = nullptr;

		float gain = 1.0f;
		float pitch = 1.0f;
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		bool loop = false;
		bool isPlaying = false;
	};
}

