#include "SceneAudioSource.h"

namespace Hogra {

	void Hogra::SceneAudioSource::Init(AudioSource* _source)
	{
		audioSource = _source;
	}

	void SceneAudioSource::Play()
	{
		audioSource->Play();
	}

	void SceneAudioSource::Stop()
	{
		audioSource->Stop();
	}

	void SceneAudioSource::SetPositionProvider(PositionProvider* provider)
	{
		positionProvider = provider;
	}

	void SceneAudioSource::SetGain(float g)
	{
		gain = g;
	}

	void SceneAudioSource::SetPitch(float p)
	{
		pitch = p;
	}

	void SceneAudioSource::SetPosition(const glm::vec3& _position)
	{
		position = _position;
	}

	void SceneAudioSource::SetVelocity(const glm::vec3& _velocity)
	{
		velocity = _velocity;
	}

	void SceneAudioSource::SetLoop(bool _loop)
	{
		loop = _loop;
	}

	void SceneAudioSource::EarlyPhysicsUpdate(float dt)
	{

	}

	void SceneAudioSource::LatePhysicsUpdate(float dt)
	{
		if (nullptr != positionProvider) {
			position = positionProvider->GetPosition();
		}
		if (nullptr == audioSource) {
			isPlaying = false;
			return;
		}
		audioSource->SetGain(gain);
		audioSource->SetPitch(pitch);
		audioSource->SetPosition(position);
		audioSource->SetVelocity(velocity);
		audioSource->SetLoop(loop);
		isPlaying = audioSource->IsPlaying();
	}
}