#include "CameraAnimation.h"

void hograengine::HeadBob::perform(void* obj, float dt)
{
	Camera* camera = (Camera*)obj;
	float amplitude = 0.05f;
	float speed = 12.0f;
	camera->setAnimationOffset(amplitude * glm::vec3(0.5f * sinf(speed * t), std::max(sinf(speed * t), -0.5f), 0.5f * sinf(speed * t)));
	t += dt;
}
