#pragma once
#include "Scene.h"

namespace Hogra {

	class SceneEvent
	{
	public:
		virtual ~SceneEvent() = default;

		virtual void Execute(Scene& scene) = 0;

	};
}

