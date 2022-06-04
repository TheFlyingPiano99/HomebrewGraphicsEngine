#include "SingletonManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetFolderPathManager.h"
#include "GUI.h"

/*
* Needs to be called before closing program!
*/
void Singleton::destroyAllInstances() {
	ControlActionManager::destroyInstance();
	Scene::destroyInstance();
	AssetFolderPathManager::destroyInstance();
	GUI::destroyInstance();
}
