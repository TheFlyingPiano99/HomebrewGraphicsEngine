#include "SingletonManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetFolderPathManager.h"
#include "GUI.h"
#include "SceneEventManager.h"

namespace hograengine {

	/*
	* Needs to be called before closing program!
	*/
	void Singleton::destroyAllInstances() {
		Scene::destroyInstance();
		ControlActionManager::destroyInstance();
		AssetFolderPath::destroyInstance();
		GUI::destroyInstance();
		SceneEventManager::destroyInstance();
	}
}