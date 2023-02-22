#include "InstanceGroup.h"
#include <algorithm>

void Hogra::InstanceGroup::GatherInstanceData()
{
	unsigned int count = objects.size();
	if (instanceData.size() != count) {
		instanceData.clear();
		instanceData.resize(count);
	}
	visibleCount = 0;
	for (int i = 0; i < count; i++) {
		if (objects[i]->IsVisible()) {
			instanceData[visibleCount].modelMatrix = objects[i]->GetModelMatrix();
			instanceData[visibleCount].invModelMatrix = objects[i]->GetInvModelMatrix();
			visibleCount++;
		}
	}
}

void Hogra::InstanceGroup::GatherInstanceDataForShadow()
{
	unsigned int count = objects.size();
	if (instanceData.size() != count) {
		instanceData.clear();
		instanceData.resize(count);
	}
	visibleCount = 0;
	for (int i = 0; i < count; i++) {
		if (objects[i]->IsVisible() && objects[i]->IsCastingShadow()) {
			instanceData[visibleCount].modelMatrix = objects[i]->GetModelMatrix();
			instanceData[visibleCount].invModelMatrix = objects[i]->GetInvModelMatrix();
			visibleCount++;
		}
	}
}

void Hogra::InstanceGroup::InjectInstanceData(const std::vector<Geometry::InstanceData>& data)
{
	instanceData = data;
	visibleCount = data.size();
}

void Hogra::InstanceGroup::Draw()
{
	if (0 == visibleCount) {
		return;
	}
	Mesh* mesh = objects[0]->GetMesh();
	mesh->Bind();
	mesh->DrawInstanced(instanceData, visibleCount);
}

void Hogra::InstanceGroup::DrawShadow()
{
	if (objects.size() == 0 || !objects[0]->IsCastingShadow()) {
		return;
	}
	Mesh* mesh = objects[0]->GetMesh();
	// Not binding the mesh, to allow use of other program!
	mesh->DrawInstanced(instanceData, visibleCount);
}

void Hogra::InstanceGroup::Optimalize(const Camera& camera)
{
	if (optimalizationCounter > 500) {
		optimalizationCounter = 0;

		std::sort(objects.begin(), objects.end(), [&camera](SceneObject const* obj1, SceneObject const* obj2) {
			return glm::length(obj1->GetPosition() - camera.GetPosition()) < glm::length(obj2->GetPosition() - camera.GetPosition());
		});
	}
	else {
		optimalizationCounter++;
	}
}
