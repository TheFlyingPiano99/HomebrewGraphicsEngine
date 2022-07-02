#pragma once
#include "Collider.h"
#include "AABBCollider.h"

namespace hograengine {

	// The max number of colliders in a leaf group
#define MAX_COLLIDER_COUNT 10

	/*
	* Encapsulates near colliders to reduce the cost of collision check
	* Uses R-tree algorithm
	*/
	class ColliderGroup
	{
	public:
		ColliderGroup() = default;
		
		~ColliderGroup() {
			for (auto& group : subGroups) {
				delete group;
			}
			// Deleting colliders is not the responsibility of the group
		}

		void updateAABB();

		void expandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax);

		void addCollider(Collider* collider);

		void removeCollider(Collider* collider);

		int getLoad();

		const std::vector<Collider*>& getColliders() const {
			return colliders;
		}

		const std::vector<ColliderGroup*>& getSubGroups() const {
			return subGroups;
		}

		const AABBCollider* getAABB() const {
			return &aabb;
		}

		void collide(const ColliderGroup* group);

		void collide(const Collider* collider);

		void selfCollide();

		const glm::vec3& getMin() const {
			return aabb.getMin();
		}

		void setMin(const glm::vec3& _min) {
			aabb.setMin(_min);
		}

		const glm::vec3& getMax() const {
			return aabb.getMax();
		}

		void setMax(const glm::vec3& _max) {
			aabb.setMax(_max);
		}

		void setSuperGroup(ColliderGroup* super) {
			superGroup = super;
		}

		void updateAABBFromColliders();

		void updateAABBFromSubGroups();

		void print(int intend);

	private:
		AABBCollider aabb;
		ColliderGroup* superGroup = nullptr;
		std::vector<ColliderGroup*> subGroups;
		std::vector<Collider*> colliders;

	};
}

