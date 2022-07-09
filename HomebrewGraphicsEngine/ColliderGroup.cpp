#include "ColliderGroup.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/glm.hpp>
#include <glm/gtx/transform.hpp>

float hograengine::ColliderGroup::getExpansion(const Collider* collider)
{
	glm::vec3 expansion = glm::vec3(0.0f);
	glm::vec3 min = getMin();
	glm::vec3 max = getMax();
	expansion.x = std::max(std::max(0.0f, min.x - collider->getAABBMin().x), std::max(0.0f, collider->getAABBMax().x - max.x));
	expansion.y = std::max(std::max(0.0f, min.y - collider->getAABBMin().y), std::max(0.0f, collider->getAABBMax().y - max.y));
	expansion.z = std::max(std::max(0.0f, min.z - collider->getAABBMin().z), std::max(0.0f, collider->getAABBMax().z - max.z));
	return length(expansion);
}

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
		if (colliders.size() > MAX_COLLIDER_COUNT && level < MAX_DEPTH_LEVEL) {	// Divide group into subgroups
			subGroups.push_back(new ColliderGroup(this, level + 1));
			subGroups.push_back(new ColliderGroup(this, level + 1));
			for (int i = 0; i < 2; i++) {	// First few in separate groups
				int groupIdx = i % 2;
				subGroups[groupIdx]->addCollider(colliders[i]);
			}
			for (int i = 2; i < colliders.size(); i++) {	// The remaining in the least expanding groups
				putInLeastExpandingSubGroup(colliders[i]);
			}
			colliders.clear();
		}
		else {
			expandAABB(collider->getAABBMin(), collider->getAABBMax());
		}
	}
	else {	// Not leaf group
		putInLeastExpandingSubGroup(collider);
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
				// Not urgent, because the tree is getting rebuit regularly
			}
		}
	}
}

int hograengine::ColliderGroup::getLoad() {
	return std::max(colliders.size(), subGroups.size());
}

void hograengine::ColliderGroup::clear()
{
	for (auto& group : subGroups) {
		delete group;
	}
	aabb.setMin(glm::vec3(0, 0, 0));
	aabb.setMax(glm::vec3(0, 0, 0));
	subGroups.clear();
	colliders.clear();
	// Deleting colliders is not the responsibility of the group!
}

void hograengine::ColliderGroup::putInLeastExpandingSubGroup(Collider* collider)
{
	float minExpansion;
	ColliderGroup* minGroup = nullptr;
	for (auto& group : subGroups) {	// Find subGroup least expanded by the new collider
		float currentExpansion = group->getExpansion(collider);
		if (nullptr == minGroup || currentExpansion < minExpansion) {
			minExpansion = currentExpansion;
			minGroup = group;
		}
	}
	minGroup->addCollider(collider);
	expandAABB(minGroup->getMin(), minGroup->getMax());
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

void hograengine::ColliderGroup::collide(Collider* collider) {
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

void hograengine::ColliderGroup::gatherInstanceDataForDebug(std::vector<Geometry::InstanceData>& data)
{
	glm::vec3 min = aabb.getMin();
	glm::vec3 max = aabb.getMax();
	glm::vec3 center = (min + max) / 2.0f;
	Geometry::InstanceData d;
	d.modelMatrix = glm::translate(center) * glm::scale((max - min) / 2.0f);
	data.push_back(d);
	for (auto& group : subGroups) {
		group->gatherInstanceDataForDebug(data);
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

void hograengine::ColliderGroup::print() {
	for (int i = 0; i < level; i++) {
		std::cout << "| ";
	}
	glm::vec3 min = getMin();
	glm::vec3 max = getMax();
	std::cout << "Group: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")] " << subGroups.size()  << std::endl;
	for (auto& group : subGroups) {
		group->print();
	}
	for (auto& coll : colliders) {
		min = coll->getAABBMin();
		max = coll->getAABBMax();
		for (int i = 0; i < level + 1; i++) {
			std::cout << "| ";
		}
		std::cout << "Collider: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")]" << std::endl;
	}
}
