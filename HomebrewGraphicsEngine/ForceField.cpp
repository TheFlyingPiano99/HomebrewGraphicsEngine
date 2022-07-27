#include "ForceField.h"

namespace Hogra {

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

	void CentralForceField::Control(float dt)
	{
	}

	void CentralForceField::Update(float dt)
	{
	}
}