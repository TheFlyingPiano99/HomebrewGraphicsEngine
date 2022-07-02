#pragma once
#include "Collider.h"
#include "AABBCollider.h"

namespace hograengine {

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

		void updateAABB() {
			if (subGroups.empty()) {
				updateAABBFromColliders();
			}
			else {
				for (auto& group : subGroups) {
					group->updateAABB();
				}
				updateAABBFromSubGroups();
			}
		}

		void expandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax) {
			glm::vec3 prevMin = aabb.getMin();
			glm::vec3 prevMax = aabb.getMax();
			glm::vec3 newMin;
			glm::vec3 newMax;
			newMin.x = (candidateMin.x < prevMin.x) ? candidateMin.x : prevMin.x;
			newMin.y = (candidateMin.y < prevMin.y) ? candidateMin.y : prevMin.y;
			newMin.z = (candidateMin.z < prevMin.z) ? candidateMin.z : prevMin.z;
			newMax.x = (candidateMax.x > prevMax.x) ? candidateMax.x : prevMax.x;
			newMax.y = (candidateMax.y > prevMax.y) ? candidateMax.y : prevMax.y;
			newMax.z = (candidateMax.z > prevMax.z) ? candidateMax.z : prevMax.z;
			aabb.setMin(newMin);
			aabb.setMax(newMax);
		}

		void addCollider(Collider* collider) {
			if (subGroups.empty()) {	// Leaf group
				colliders.push_back(collider);
				if (colliders.size() > MAX_COLLIDER_COUNT) {
					subGroups.push_back(new ColliderGroup());
					subGroups.push_back(new ColliderGroup());
					for (int i = 0; i < colliders.size(); i++) {	// Distribute colliders betwean subGroups
						int groupIdx = i % 2;
						subGroups[groupIdx]->addCollider(colliders[i]);
					}
					colliders.clear();
				}
				expandAABB(collider->getAABBMin(), collider->getAABBMax());
			}
			else {	// Not leaf group
				int minLoad;
				ColliderGroup* minGroup = nullptr;
				for (auto& group : subGroups) {	// Find subGroup with least load
					int currentLoad = group->getLoad();
					if (nullptr == minGroup || currentLoad < minLoad) {
						minLoad = currentLoad;
						minGroup = group;
					}
				}
				minGroup->addCollider(collider);
				expandAABB(minGroup->getMin(), minGroup->getMax());
			}
		}

		void removeCollider(Collider* collider) {
			if (!subGroups.empty()) {
				for (auto& group : subGroups) {
					group->removeCollider(collider);
				}
			}
			else {
				const auto& iter = std::find(colliders.begin(), colliders.end(), collider);
				if (iter != colliders.end()) {
					colliders.erase(iter);
					if (colliders.empty()) {
						// TODO: Remove colliderGroup from super.
					}
				}
			}
		}

		int getLoad() {
			return std::max(colliders.size(), subGroups.size());
		}

		const std::vector<Collider*>& getColliders() const {
			return colliders;
		}

		const AABBCollider* getAABB() const {
			return &aabb;
		}

		void collide(const ColliderGroup* group) {
			if (aabb.testCollision(group->getAABB())) {
				// TODO: recursion
			}
		}

		void selfCollide() {
			if (!subGroups.empty()) {
				for (int i = 0; i < subGroups.size() - 1; i++) {
					for (int j = i + 1; j < subGroups.size(); j++) {
						subGroups[i]->selfCollide();
						subGroups[i]->collide(subGroups[j]);
					}
				}
				subGroups[subGroups.size() - 1]->selfCollide();
			}
			else {
				for (int i = 0; i < colliders.size() - 1; i++) {
					for (int j = i + 1; j < colliders.size(); j++) {
						colliders[i]->collide(*colliders[j]);
					}
				}
			}
		}

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

		void updateAABBFromColliders() {
			bool isMinSet = false;
			bool isMaxSet = false;
			glm::vec3 newMin;
			glm::vec3 newMax;
			for (const auto& collider : colliders) {
				glm::vec3 currentMin = collider->getAABBMin();
				if (!isMinSet) {
					newMin = currentMin;
					isMinSet = true;
				}
				else {
					if (currentMin.x < newMin.x) {
						newMin.x = currentMin.x;
					}
					if (currentMin.y < newMin.y) {
						newMin.y = currentMin.y;
					}
					if (currentMin.z < newMin.z) {
						newMin.z = currentMin.z;
					}
				}
				glm::vec3 currentMax = collider->getAABBMax();
				if (!isMaxSet) {
					newMax = currentMax;
					isMaxSet = true;
				}
				else {
					if (currentMax.x > newMax.x) {
						newMax.x = currentMax.x;
					}
					if (currentMax.y > newMax.y) {
						newMax.y = currentMax.y;
					}
					if (currentMax.z > newMax.z) {
						newMax.z = currentMax.z;
					}
				}
			}
			aabb.setMin(newMin);
			aabb.setMax(newMax);
		}

		void updateAABBFromSubGroups() {
			bool isMinSet = false;
			bool isMaxSet = false;
			glm::vec3 newMin;
			glm::vec3 newMax;
			for (const auto& group : subGroups) {
				glm::vec3 currentMin = group->getMin();
				if (!isMinSet) {
					newMin = currentMin;
					isMinSet = true;
				}
				else {
					if (currentMin.x < newMin.x) {
						newMin.x = currentMin.x;
					}
					if (currentMin.y < newMin.y) {
						newMin.y = currentMin.y;
					}
					if (currentMin.z < newMin.z) {
						newMin.z = currentMin.z;
					}
				}
				glm::vec3 currentMax = group->getMax();
				if (!isMaxSet) {
					newMax = currentMax;
					isMaxSet = true;
				}
				else {
					if (currentMax.x > newMax.x) {
						newMax.x = currentMax.x;
					}
					if (currentMax.y > newMax.y) {
						newMax.y = currentMax.y;
					}
					if (currentMax.z > newMax.z) {
						newMax.z = currentMax.z;
					}
				}
			}
			aabb.setMin(newMin);
			aabb.setMax(newMax);
		}

	private:
		AABBCollider aabb;
		ColliderGroup* superGroup = nullptr;
		std::vector<ColliderGroup*> subGroups;
		std::vector<Collider*> colliders;

	};
}

