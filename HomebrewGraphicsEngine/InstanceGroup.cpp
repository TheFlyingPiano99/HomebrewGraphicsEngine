#include "InstanceGroup.h"
#include <algorithm>

void hograengine::InstanceGroup::gatherInstanceData()
{
	unsigned int count = objects.size();
	if (instanceData.size() != count) {
		instanceData.clear();
		instanceData.resize(count);
	}
	for (int i = 0; i < count; i++) {
		instanceData[i].modelMatrix = objects[i]->getModelMatrix();
		instanceData[i].invModelMatrix = objects[i]->getInvModelMatrix();
	}
}

void hograengine::InstanceGroup::injectInstanceData(const std::vector<Geometry::InstanceData>& data)
{
	instanceData = data;
}

void hograengine::InstanceGroup::draw()
{
	if (objects.size() == 0) {
		return;
	}
	Mesh* mesh = objects[0]->getMesh();
	mesh->Bind();
	mesh->DrawInstanced(instanceData);
}

void hograengine::InstanceGroup::drawShadow()
{
	if (objects.size() == 0) {
		return;
	}
	Mesh* mesh = objects[0]->getMesh();
	mesh->DrawInstanced(instanceData);
}

void hograengine::InstanceGroup::optimalize(const Camera& camera)
{
	if (optimalizationCounter > 500) {
		optimalizationCounter = 0;

		std::sort(objects.begin(), objects.end(), [&camera](SceneObject const* obj1, SceneObject const* obj2) {
			return glm::length(obj1->getPosition() - camera.getEyePos()) < glm::length(obj2->getPosition() - camera.getEyePos());
		});
	}
	else {
		optimalizationCounter++;
	}
}
