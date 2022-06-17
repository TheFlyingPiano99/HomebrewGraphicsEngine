#include "ForceField.h"

namespace hograengine {

	void HomogeneForceField::control(float dt)
	{
		for (auto& listener : listeners) {
			float m = listener->getMass();
			if (!isnan(m)) {
				glm::vec3 f = direction * strength * m;
				listener->applyTransientForce(f);
			}
		}
	}

	void HomogeneForceField::update(float dt)
	{
	}

	void CentralForceField::control(float dt)
	{
	}

	void CentralForceField::update(float dt)
	{
	}
}