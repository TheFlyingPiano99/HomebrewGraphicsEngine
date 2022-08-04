#include "CollisionManager.h"
#include <algorithm>
#include "GeometryFactory.h"

void Hogra::CollisionManager::AddCollider(Collider* collider, const std::string& colliderGroupName) {
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
				collider->AddToColliderGroup(i);
				groupAlreadyExists = true;
				break;
			}
		}
		if (!groupAlreadyExists) {
			names.push_back(colliderGroupName);
			collider->AddToColliderGroup(names.size() - 1);
		}
	}
	root.AddCollider(collider);
}

void Hogra::CollisionManager::RemoveCollider(Collider* collider) {
	const auto& iter = std::find(colliders.begin(), colliders.end(), collider);
	if (colliders.end() != iter) {
		colliders.erase(iter);
		root.RemoveCollider(collider);
	}
}

void Hogra::CollisionManager::Collide() {
	if (useSpatialTree) {
		root.SelfCollide();
	}
	else {
		for (int i = 0; i < colliders.size() - 1; i++) {
			for (int j = i + 1; j < colliders.size(); j++) {
				colliders[i]->Collide(colliders[j]);
			}
		}
	}
}


void Hogra::CollisionManager::Update() {
	static int counter0 = 0;
	static int counter1 = 0;
	if (counter0 == 1) {
		root.UpdateAABB();
		counter0 = 0;
	}
	else {
		counter0++;
	}
	if (counter1 == 1000) {
		struct {
			bool operator()(const Collider* a, const Collider* b) const { return length(a->GetAABBMax() - a->GetAABBMin()) < length(b->GetAABBMax() - b->GetAABBMin()); }
		} customLess;
		root.Clear();
		std::sort(colliders.begin(), colliders.end(), customLess);
		for (auto* collider : colliders) {
			root.AddCollider(collider);
		}
		counter1 = 0;
	}
	else {
		counter1++;
	}
}

void Hogra::CollisionManager::SetUseSpatialTree(bool b) {
	useSpatialTree = b;
}

void Hogra::CollisionManager::InitDebug()
{
	shaderProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.vert"), 
		"", 
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.frag"));
	auto* geometry = GeometryFactory::getInstance()->getWireframeCube();
	auto* material = Material::Instantiate();
	material->Init(&shaderProgram);
	auto* mesh = Mesh::Instantiate();
	mesh->Init(material, geometry);
	mesh->setDepthTest(false);
	auto* obj = SceneObject::Instantiate();
	obj->Init(mesh);
	instanceGroup.addObject(obj);
}

void Hogra::CollisionManager::DrawDebug()
{
	std::vector<Geometry::InstanceData> data;
	root.GatherInstanceDataForDebug(data);
	instanceGroup.injectInstanceData(data);
	instanceGroup.Draw();
}
