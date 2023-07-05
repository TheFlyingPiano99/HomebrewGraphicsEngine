#include "ForceField.h"

namespace Hogra {

	void HomogeneForceField::EarlyPhysicsUpdate(float dt_sec)
	{
		for (auto& listener : listeners) {
			float m = listener->GetMass();
			if (!isnan(m)) {
				glm::vec3 f = direction * strength * m;
				listener->ApplyTransientForce(f);
			}
		}
	}

	void HomogeneForceField::LatePhysicsUpdate(float dt_sec)
	{
	}


	void CentralForceField::EarlyPhysicsUpdate(float dt_sec)
	{
	}

	void CentralForceField::LatePhysicsUpdate(float dt_sec)
	{
	}
}