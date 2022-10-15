#pragma once
#include "Collider.h"
#include "AABBCollider.h"

namespace Hogra {

	// The max number of colliders in a leaf group
#define MAX_COLLIDER_COUNT 10
#define MAX_DEPTH_LEVEL 10

	/*
	* Encapsulates near colliders to reduce the cost of collision check
	* Uses R-tree algorithm
	*/
	class ColliderGroup
	{
	public:
		ColliderGroup() {
		}

		void Init(ColliderGroup* _parent, int _level = 0) {
			parent = _parent;
			level = _level;
			aabb.Init();
		}
		
		~ColliderGroup() {
			for (auto& group : subGroups) {
				Allocator::Delete(group);
			}
			// Deleting colliders is not the responsibility of the group
		}

		float GetExpansion(const Collider* collider);

		void UpdateAABB();

		void ExpandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax);

		void AddCollider(Collider* collider);

		void RemoveCollider(Collider* collider);

		int GetLoad();

		void Clear();

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

		void Collide(const ColliderGroup* group);

		void Collide(Collider* collider);

		void SelfCollide();

		void GatherInstanceDataForDebug(std::vector<Geometry::InstanceData>& data);

		const glm::vec3& getMin() const {
			return aabb.getMin();
		}

		void SetMin(const glm::vec3& _min) {
			aabb.SetMin(_min);
		}

		const glm::vec3& GetMax() const {
			return aabb.GetMax();
		}

		void SetMax(const glm::vec3& _max) {
			aabb.SetMax(_max);
		}

		void SetParent(ColliderGroup* super) {
			parent = super;
		}

		void UpdateAABBFromColliders();

		void UpdateAABBFromSubGroups();

		void Print();

	private:
		int level = 0;
		AABBCollider aabb;
		ColliderGroup* parent = nullptr;
		std::vector<ColliderGroup*> subGroups;
		std::vector<Collider*> colliders;

	};
}

