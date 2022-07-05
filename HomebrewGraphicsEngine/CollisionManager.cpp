#include "CollisionManager.h"
#include <algorithm>

void hograengine::CollisionManager::addCollider(Collider* collider, const std::string& colliderGroupName) {
	const auto& iter = std::find(colliders.begin(), colliders.end(), collider);
	if (colliders.end() != iter) {	// Prevent from adding the same collider
		return;
	}
	colliders.push_back(collider);
	static std::vector<std::string> names;
	if (!colliderGroupName.empty()) {
		bool groupAlreadyExists = false;
		for (int i = 0; i < names.size(); i++) {
			if (colliderGroupName.compare(names[i]) == 0) {
				collider->addToColliderGroup(i);
				groupAlreadyExists = true;
				break;
			}
		}
		if (!groupAlreadyExists) {
			names.push_back(colliderGroupName);
			collider->addToColliderGroup(names.size() - 1);
		}
	}
	root.addCollider(collider);
}

void hograengine::CollisionManager::removeCollider(Collider* collider) {
	root.removeCollider(collider);
}

void hograengine::CollisionManager::collide() {
	if (useSpatialTree) {
		root.selfCollide();
	}
	else {
		for (int i = 0; i < colliders.size() - 1; i++) {
			for (int j = i + 1; j < colliders.size(); j++) {
				colliders[i]->collide(colliders[j]);
			}
		}
	}
}


void hograengine::CollisionManager::update() {
	static int counter0 = 0;
	static int counter1 = 2000;
	if (counter0 == 10) {
		root.updateAABB();
		counter0 = 0;
	}
	else {
		counter0++;
	}
	if (counter1 == 2000) {
		struct {
			bool operator()(const Collider* a, const Collider* b) const { return length(a->getAABBMax() - a->getAABBMin()) < length(b->getAABBMax() - b->getAABBMin()); }
		} customLess;
		root.clear();
		std::sort(colliders.begin(), colliders.end(), customLess);
		for (auto* collider : colliders) {
			root.addCollider(collider);
		}
		counter1 = 0;
	}
	else {
		counter1++;
	}
}

void hograengine::CollisionManager::setUseSpatialTree(bool b) {
	useSpatialTree = b;
}
