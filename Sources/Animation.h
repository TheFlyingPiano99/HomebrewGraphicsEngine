#pragma once
#include<glm/glm.hpp>
#include "MemoryManager.h"

namespace Hogra {

	class Animation
	{
	ALLOCATOR_CONSTRUCTIBLE
	
	public:
		virtual void perform(void* obj, float dt_sec) = 0;
		void reset();
	
	protected:
		float t = 0.0f; // sec
	};


	class GoAround : public Animation {
		float angularVelocity = 1.0f;
		float radius;
		glm::vec3 lookAt;

	public:
		GoAround(float _r, float _angVel, glm::vec3 _center = glm::vec3(0.0f)) : radius(_r), angularVelocity(_angVel), lookAt(_center) {
		}

		void perform(void* obj, float dt_sec) override;
	};

}