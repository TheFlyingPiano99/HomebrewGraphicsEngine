#include "ForceField.h"

namespace Hogra {

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


	void CentralForceField::EarlyPhysicsUpdate(float dt)
	{
	}

	void CentralForceField::Update()
	{
	}
}