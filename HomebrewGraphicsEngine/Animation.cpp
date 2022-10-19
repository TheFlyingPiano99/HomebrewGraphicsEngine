#include "Animation.h"
#include "HograTime.h"

namespace Hogra {

	void Animation::reset() {
		t = 0.0f;
	}

	void GoAround::perform(void* obj, float dt) {
		t += dt;
		//((SceneObject*)obj)->setPosition(center + glm::vec3(radius * sin(angularVelocity * t), 0, radius * cos(angularVelocity * t)));
	}
}
