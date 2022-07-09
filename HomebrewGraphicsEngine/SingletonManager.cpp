#include "SingletonManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetFolderPathManager.h"
#include "GUI.h"
#include "SceneEventManager.h"
#include "MaterialFactory.h"
#include "ShaderProgramFactory.h"
#include "SceneManager.h"
#include "SceneFactory.h"

namespace hograengine {

	/*
	* Needs to be called before closing program!
	*/
	void Singleton::destroyAllInstances() {
		GUI::destroyInstance();
		Scene::destroyInstance();
		AssetFolderPathManager::destroyInstance();
		ControlActionManager::destroyInstance();
		SceneEventManager::destroyInstance();
		MaterialFactory::destroyInstance();
		GeometryFactory::destroyInstance();
		ShaderProgramFactory::destroyInstance();
		SceneManager::destroyInstance();
		SceneFactory::destroyInstance();
	}
}