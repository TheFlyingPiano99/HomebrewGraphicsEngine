#include "ForceField.h"

namespace Hogra {
	HomogeneForceField* HomogeneForceField::Instantiate()
	{
		auto instance = new HomogeneForceField();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}
	void HomogeneForceField::Control(float dt)
	{
		for (auto& listener : listeners) {
			float m = listener->getMass();
			if (!isnan(m)) {
				glm::vec3 f = direction * strength * m;
				listener->applyTransientForce(f);
			}
		}
	}

	void HomogeneForceField::Update(float dt)
	{
	}

	CentralForceField* CentralForceField::Instantiate()
	{
		auto* instance = new CentralForceField();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}

	void CentralForceField::Control(float dt)
	{
	}

	void CentralForceField::Update(float dt)
	{
	}
}