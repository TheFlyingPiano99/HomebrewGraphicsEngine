#pragma once
#include "MemoryManager.h"
#include "SceneObject.h"
#include "Scene.h"
#include <string>

namespace Hogra {

	class SceneObjectFactory
	{
	public:

		static SceneObjectFactory* GetInstance();

		SceneObject* Create2DSpriteObject(const std::string& texturePath, OrientationProvider* orientationProvider);

	private:
		static SceneObjectFactory* instance;
	};

}

