#include "SceneManager.h"
#include "SceneFactory.h"

namespace hograengine {
	SceneManager* SceneManager::instance = nullptr;

	SceneManager* SceneManager::getInstance() {
		if (nullptr == instance) {
			instance = new SceneManager();
		}
		return instance;
	}
	void SceneManager::init(int contextWidth, int contextHeight)
	{
		SceneFactory::getInstance()->initDefaultScene(contextWidth, contextHeight);
	}
	void SceneManager::draw()
	{
		Scene::getInstance()->draw();
	}
}