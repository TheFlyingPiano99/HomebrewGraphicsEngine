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

namespace Hogra {

	/*
	* Needs to be called before closing program!
	*/
	void Singleton::destroyAllInstances() {
		GUI::DestroyInstance();
		AssetFolderPathManager::DestroyInstance();
		ControlActionManager::DestroyInstance();
		SceneEventManager::DestroyInstance();
		MaterialFactory::DestroyInstance();
		GeometryFactory::DestroyInstance();
		ShaderProgramFactory::DestroyInstance();
		SceneManager::DestroyInstance();
		SceneFactory::DestroyInstance();
	}
}