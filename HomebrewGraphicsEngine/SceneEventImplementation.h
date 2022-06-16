#pragma once
#include "SceneEvent.h"

#include "Collider.h"
namespace hograengine {

	class CollisionEvent : public SceneEvent
	{
	public:
		CollisionEvent(const Collider* _collider1, const Collider* _collider2) : collider1(_collider1), collider2(_collider2) {}

		~CollisionEvent() override = default;

		void execute(float dt) override;

	private:

		const Collider* collider1;
		const Collider* collider2;
	};
}