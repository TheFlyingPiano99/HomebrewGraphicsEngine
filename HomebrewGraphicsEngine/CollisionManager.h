#pragma once
#include "ColliderGroup.h"

namespace hograengine {
	class CollisionManager
	{
	public:
		CollisionManager() = default;

		void addCollider(Collider* collider, const std::string& colliderGroupName);

		void removeCollider(Collider* collider);

		void collide();

		void update();

		void setUseSpatialTree(bool b);

	private:
		ColliderGroup root;
		std::vector<Collider*> colliders;
		bool useSpatialTree = true;
	};
}

