#pragma once
#include "Scene.h"
#include "ObserveObjectControl.h"
#include "MemoryManager.h"
#include<glm/glm.hpp>

namespace Hogra {
	class SceneFactory
	{
		friend class Allocator;
	public:

		static SceneFactory* getInstance();

		static void DestroyInstance();

		Scene* CreateVoxelDemoScene(int contextWidth, int contextHeight, int argc = 0, char* argv[] = nullptr);

		void InitVoxelCaption(Scene* scene, const char* dataSetName);

		ObserveObjectControl* InitObjectObserverControl(Scene* scene, Volumetric::VolumeObject* volumeObject);

	private:
		SceneFactory() = default;
		static SceneFactory* instance;
	};
}
