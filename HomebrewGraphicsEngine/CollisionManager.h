#pragma once
#include "ColliderGroup.h"
#include "InstanceGroup.h"

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

		void initDebug();

		void drawDebug();

	private:
		ColliderGroup root = ColliderGroup(nullptr);
		std::vector<Collider*> colliders;
		bool useSpatialTree = true;
		InstanceGroup instanceGroup;
	};
}

