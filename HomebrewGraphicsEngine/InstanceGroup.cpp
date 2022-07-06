#include "InstanceGroup.h"

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
