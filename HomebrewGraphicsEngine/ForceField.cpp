#include "ForceField.h"

namespace Hogra {
	HomogeneForceField* HomogeneForceField::Instantiate()
	{
		auto instance = new HomogeneForceField();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}
	void HomogeneForceField::EarlyPhysicsUpdate(float dt)
	{
		for (auto& listener : listeners) {
			float m = listener->getMass();
			if (!isnan(m)) {
				glm::vec3 f = direction * strength * m;
				listener->applyTransientForce(f);
			}
		}
	}

	void HomogeneForceField::Update()
	{
	}

	CentralForceField* CentralForceField::Instantiate()
	{
		auto* instance = new CentralForceField();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}

	void CentralForceField::EarlyPhysicsUpdate(float dt)
	{
	}

	void CentralForceField::Update()
	{
	}
}