#pragma once
#include<glm/glm.hpp>
namespace Hogra {

	class Animation
	{
	protected:
		float t = 0.0f;

	public:
		virtual void perform(void* obj, float dt) = 0;
		void reset();
	};


	class GoAround : public Animation {
		float angularVelocity = 1.0;
		float radius;
		glm::vec3 lookAt;

	public:
		GoAround(float _r, float _angVel, glm::vec3 _center = glm::vec3(0.0f)) : radius(_r), angularVelocity(_angVel), lookAt(_center) {
		}

		void perform(void* obj, float dt) override;
	};

}