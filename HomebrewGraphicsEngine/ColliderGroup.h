#pragma once
#include "Collider.h"
#include "AABBCollider.h"

namespace hograengine {

	// The max number of colliders in a leaf group
#define MAX_COLLIDER_COUNT 20
#define MAX_DEPTH_LEVEL 40

	/*
	* Encapsulates near colliders to reduce the cost of collision check
	* Uses R-tree algorithm
	*/
	class ColliderGroup
	{
	public:
		ColliderGroup(ColliderGroup* _parent, int _level = 0) : parent(_parent), level(_level) {}
		
		~ColliderGroup() {
			for (auto& group : subGroups) {
				delete group;
			}
			// Deleting colliders is not the responsibility of the group
		}

		float getExpansion(const Collider* collider);

		void updateAABB();

		void expandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax);

		void addCollider(Collider* collider);

		void removeCollider(Collider* collider);

		int getLoad();

		void clear();

		void putInLeastExpandingSubGroup(Collider* collider);

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

		void setParent(ColliderGroup* super) {
			parent = super;
		}

		void updateAABBFromColliders();

		void updateAABBFromSubGroups();

		void print();

	private:
		int level = 0;
		AABBCollider aabb;
		ColliderGroup* parent = nullptr;
		std::vector<ColliderGroup*> subGroups;
		std::vector<Collider*> colliders;

	};
}

