#include "ColliderGroup.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/glm.hpp>
#include <glm/gtx/transform.hpp>

float Hogra::ColliderGroup::GetExpansion(const Collider* collider)
{
	glm::vec3 expansion = glm::vec3(0.0f);
	glm::vec3 min = getMin();
	glm::vec3 max = GetMax();
	expansion.x = std::max(std::max(0.0f, min.x - collider->GetAABBMin().x), std::max(0.0f, collider->GetAABBMax().x - max.x));
	expansion.y = std::max(std::max(0.0f, min.y - collider->GetAABBMin().y), std::max(0.0f, collider->GetAABBMax().y - max.y));
	expansion.z = std::max(std::max(0.0f, min.z - collider->GetAABBMin().z), std::max(0.0f, collider->GetAABBMax().z - max.z));
	return length(expansion);
}

void Hogra::ColliderGroup::UpdateAABB() {
	if (subGroups.empty()) {
		UpdateAABBFromColliders();
	}
	else {
		for (auto& group : subGroups) {
			group->UpdateAABB();
		}
		UpdateAABBFromSubGroups();
	}
}

void Hogra::ColliderGroup::ExpandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax) {
	glm::vec3 prevMin = aabb.getMin();
	glm::vec3 prevMax = aabb.GetMax();
	glm::vec3 newMin;
	glm::vec3 newMax;
	newMin.x = (candidateMin.x < prevMin.x) ? candidateMin.x : prevMin.x;
	newMin.y = (candidateMin.y < prevMin.y) ? candidateMin.y : prevMin.y;
	newMin.z = (candidateMin.z < prevMin.z) ? candidateMin.z : prevMin.z;
	newMax.x = (candidateMax.x > prevMax.x) ? candidateMax.x : prevMax.x;
	newMax.y = (candidateMax.y > prevMax.y) ? candidateMax.y : prevMax.y;
	newMax.z = (candidateMax.z > prevMax.z) ? candidateMax.z : prevMax.z;
	aabb.SetMin(newMin);
	aabb.SetMax(newMax);
}

void Hogra::ColliderGroup::AddCollider(Collider* collider) {
	if (subGroups.empty()) {	// Leaf group
		colliders.push_back(collider);
		if (colliders.size() > MAX_COLLIDER_COUNT && level < MAX_DEPTH_LEVEL) {	// Divide group into subgroups
			auto group1 = Allocator::New<ColliderGroup>();
			group1->Init(this, level + 1);
			subGroups.push_back(group1);
			auto group2 = Allocator::New<ColliderGroup>();
			group2->Init(this, level + 1);
			subGroups.push_back(group2);
			for (int i = 0; i < 2; i++) {	// First few in separate groups
				int groupIdx = i % 2;
				subGroups[groupIdx]->AddCollider(colliders[i]);
			}
			for (int i = 2; i < colliders.size(); i++) {	// The remaining in the least expanding groups
				putInLeastExpandingSubGroup(colliders[i]);
			}
			colliders.clear();
		}
		else {
			ExpandAABB(collider->GetAABBMin(), collider->GetAABBMax());
		}
	}
	else {	// Not leaf group
		putInLeastExpandingSubGroup(collider);
	}
}

void Hogra::ColliderGroup::RemoveCollider(Collider* collider) {
	if (!subGroups.empty()) {
		for (auto& group : subGroups) {
			group->RemoveCollider(collider);
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

int Hogra::ColliderGroup::GetLoad() {
	return std::max(colliders.size(), subGroups.size());
}

void Hogra::ColliderGroup::Clear()
{
	for (auto& group : subGroups) {
		Allocator::Delete(group);
	}
	aabb.SetMin(glm::vec3(0, 0, 0));
	aabb.SetMax(glm::vec3(0, 0, 0));
	subGroups.clear();
	colliders.clear();
	// Deleting colliders is not the responsibility of the group!
}

void Hogra::ColliderGroup::putInLeastExpandingSubGroup(Collider* collider)
{
	float minExpansion;
	ColliderGroup* minGroup = nullptr;
	for (auto& group : subGroups) {	// Find subGroup least expanded by the added collider
		float currentExpansion = group->GetExpansion(collider);
		if (nullptr == minGroup || currentExpansion < minExpansion) {
			minExpansion = currentExpansion;
			minGroup = group;
		}
	}
	minGroup->AddCollider(collider);
	ExpandAABB(minGroup->getMin(), minGroup->GetMax());
}

void Hogra::ColliderGroup::Collide(const ColliderGroup* group) {
	if (aabb.TestCollision(group->getAABB())) {
		if (subGroups.empty()) {
			if (group->getSubGroups().empty()) {
				const auto& otherColliders = group->getColliders();
				for (int i = 0; i < colliders.size(); i++) {
					for (int j = 0; j < otherColliders.size(); j++) {
						colliders[i]->Collide(otherColliders[j]);
					}
				}
			}
			else {
				const auto& otherSubGroups = group->getSubGroups();
				for (int i = 0; i < colliders.size(); i++) {
					for (int j = 0; j < otherSubGroups.size(); j++) {
						otherSubGroups[j]->Collide(colliders[i]);
					}
				}
			}
		}
		else {
			if (group->getSubGroups().empty()) {
				const auto& otherColliders = group->getColliders();
				for (int i = 0; i < subGroups.size(); i++) {
					for (int j = 0; j < otherColliders.size(); j++) {
						subGroups[i]->Collide(otherColliders[j]);
					}
				}
			}
			else {
				const auto& otherSubGroups = group->getSubGroups();
				for (int i = 0; i < subGroups.size(); i++) {
					for (int j = 0; j < otherSubGroups.size(); j++) {
						subGroups[i]->Collide(otherSubGroups[j]);
					}
				}
			}
		}
	}
}

void Hogra::ColliderGroup::Collide(Collider* collider) {
	if (aabb.TestCollision(collider)) {
		if (subGroups.empty()) {
			for (auto& coll : colliders) {
				collider->Collide(coll);
			}
		}
		else {
			for (auto& group : subGroups) {
				group->Collide(collider);
			}
		}
	}
}

void Hogra::ColliderGroup::SelfCollide() {
	if (subGroups.empty()) {
		for (int i = 0; i < colliders.size() - 1 && colliders.size() > 0; i++) {
			for (int j = i + 1; j < colliders.size(); j++) {
				colliders[i]->Collide(colliders[j]);
			}
		}
	}
	else {
		for (int i = 0; i < subGroups.size() - 1; i++) {
			for (int j = i + 1; j < subGroups.size(); j++) {

				subGroups[i]->SelfCollide();
				subGroups[i]->Collide(subGroups[j]);
			}
		}
		subGroups[subGroups.size() - 1]->SelfCollide();
	}
}

void Hogra::ColliderGroup::GatherInstanceDataForDebug(std::vector<Geometry::InstanceData>& data)
{
	glm::vec3 min = aabb.getMin();
	glm::vec3 max = aabb.GetMax();
	glm::vec3 lookAt = (min + max) / 2.0f;
	Geometry::InstanceData d;
	d.modelMatrix = glm::translate(lookAt) * glm::scale((max - min) / 2.0f);
	data.push_back(d);
	for (auto& group : subGroups) {
		group->GatherInstanceDataForDebug(data);
	}
}

void Hogra::ColliderGroup::UpdateAABBFromColliders() {
	bool isMinSet = false;
	bool isMaxSet = false;
	glm::vec3 newMin;
	glm::vec3 newMax;
	for (const auto& collider : colliders) {
		glm::vec3 currentMin = collider->GetAABBMin();
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
		glm::vec3 currentMax = collider->GetAABBMax();
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
	aabb.SetMin(newMin);
	aabb.SetMax(newMax);
}

void Hogra::ColliderGroup::UpdateAABBFromSubGroups() {
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
		glm::vec3 currentMax = group->GetMax();
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
	aabb.SetMin(newMin);
	aabb.SetMax(newMax);
}

void Hogra::ColliderGroup::Print() {
	for (int i = 0; i < level; i++) {
		std::cout << "| ";
	}
	glm::vec3 min = getMin();
	glm::vec3 max = GetMax();
	std::cout << "Group: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")] " << subGroups.size()  << std::endl;
	for (auto& group : subGroups) {
		group->Print();
	}
	for (auto& coll : colliders) {
		min = coll->GetAABBMin();
		max = coll->GetAABBMax();
		for (int i = 0; i < level + 1; i++) {
			std::cout << "| ";
		}
		std::cout << "Collider: [(" << min.x << ", " << min.y << ", " << min.z << ")\t(" << max.x << ", " << max.y << "," << max.z << ")]" << std::endl;
	}
}
