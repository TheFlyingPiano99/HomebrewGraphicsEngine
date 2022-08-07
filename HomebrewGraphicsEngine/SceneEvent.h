#pragma once
#include "Scene.h"

namespace Hogra {

	class SceneEvent
	{
	public:
		virtual ~SceneEvent() = default;

		virtual void execute(Scene* scene, float dt) = 0;

	};
}

