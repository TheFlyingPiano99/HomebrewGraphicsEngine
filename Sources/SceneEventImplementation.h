#pragma once
#include "SceneEvent.h"

#include "Collider.h"
namespace Hogra {

	class CollisionEvent : public SceneEvent
	{
	public:
		CollisionEvent() = default;

		void Init(const Collider* _collider1, const Collider* _collider2) {
			collider1 = _collider1;
			collider2 = _collider2;
		}

		~CollisionEvent() override = default;

		void Execute(Scene& scene) override;

	private:

		const Collider* collider1;
		const Collider* collider2;
	};

	class RestartSceneEvent : public SceneEvent
	{
	public:

		void Execute(Scene& scene) override;

	private:
	};

}