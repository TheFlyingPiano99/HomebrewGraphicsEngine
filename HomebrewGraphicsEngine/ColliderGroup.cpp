#include "ColliderGroup.h"
#include <iostream>

void hograengine::ColliderGroup::updateAABB() {
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

void hograengine::ColliderGroup::expandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax) {
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

void hograengine::ColliderGroup::addCollider(Collider* collider) {
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

void hograengine::ColliderGroup::removeCollider(Collider* collider) {
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

int hograengine::ColliderGroup::getLoad() {
	return std::max(colliders.size(), subGroups.size());
}

void hograengine::ColliderGroup::collide(const ColliderGroup* group) {
	if (aabb.testCollision(group->getAABB())) {
		if (subGroups.empty()) {
			if (group->getSubGroups().empty()) {
				const auto& otherColliders = group->getColliders();
				for (int i = 0; i < colliders.size(); i++) {
					for (int j = 0; j < otherColliders.size(); j++) {
						colliders[i]->collide(otherColliders[j]);
					}
				}
			}
			else {
				const auto& otherSubGroups = group->getSubGroups();
				for (int i = 0; i < colliders.size(); i++) {
					for (int j = 0; j < otherSubGroups.size(); j++) {
						otherSubGroups[j]->collide(colliders[i]);
					}
				}
			}
		}
		else {
			if (group->getSubGroups().empty()) {
				const auto& otherColliders = group->getColliders();
				for (int i = 0; i < subGroups.size(); i++) {
					for (int j = 0; j < otherColliders.size(); j++) {
						subGroups[i]->collide(otherColliders[j]);
					}
				}
			}
			else {
				const auto& otherSubGroups = group->getSubGroups();
				for (int i = 0; i < subGroups.size(); i++) {
					for (int j = 0; j < otherSubGroups.size(); j++) {
						subGroups[i]->collide(otherSubGroups[j]);
					}
				}
			}
		}
	}
}

void hograengine::ColliderGroup::collide(const Collider* collider) {
	if (aabb.testCollision(collider)) {
		if (subGroups.empty()) {
			for (auto& coll : colliders) {
				collider->collide(coll);
			}
		}
		else {
			for (auto& group : subGroups) {
				group->collide(collider);
			}
		}
	}
}

void hograengine::ColliderGroup::selfCollide() {
	if (subGroups.empty()) {
		for (int i = 0; i < colliders.size() - 1; i++) {
			for (int j = i + 1; j < colliders.size(); j++) {
				colliders[i]->collide(colliders[j]);
			}
		}
	}
	else {
		for (int i = 0; i < subGroups.size() - 1; i++) {
			for (int j = i + 1; j < subGroups.size(); j++) {
				subGroups[i]->selfCollide();
				subGroups[i]->collide(subGroups[j]);
			}
		}
		subGroups[subGroups.size() - 1]->selfCollide();
	}
}

void hograengine::ColliderGroup::updateAABBFromColliders() {
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

void hograengine::ColliderGroup::updateAABBFromSubGroups() {
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

void hograengine::ColliderGroup::print(int intend) {
	for (int i = 0; i < intend; i++) {
		std::cout << "\t";
	}
	glm::vec3 min = getMin();
	glm::vec3 max = getMax();
	std::cout << "Group: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")]" << std::endl;
	for (auto& group : subGroups) {
		group->print(intend + 1);
	}
	for (auto& coll : colliders) {
		min = coll->getAABBMin();
		max = coll->getAABBMax();
		for (int i = 0; i < intend + 1; i++) {
			std::cout << "\t";
		}
		std::cout << "Collider: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")]" << std::endl;
	}
}
