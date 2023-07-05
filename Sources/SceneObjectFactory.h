#pragma once
#include "MemoryManager.h"
#include "SceneObject.h"
#include "Scene.h"
#include <string>

namespace Hogra {

	class SceneObjectFactory
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		static SceneObjectFactory* GetInstance();

		SceneObject* Create2DSpriteObject(const std::filesystem::path& texturePath, OrientationProvider* orientationProvider = nullptr);

	private:
		static SceneObjectFactory* instance;
	};

}

