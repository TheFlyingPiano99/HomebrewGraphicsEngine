#include "Caption.h"

Hogra::Caption* Hogra::Caption::Instantiate()
{
	auto* instance = new Caption();
	heapAllocatedInstances.push_back(instance);
	return instance;
}

void Hogra::Caption::Control(float dt)
{
}

void Hogra::Caption::Update(float dt)
{
}
