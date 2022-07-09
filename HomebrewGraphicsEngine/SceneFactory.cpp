#include "SceneFactory.h"

namespace hograengine {
	SceneFactory* SceneFactory::instance = nullptr;

	SceneFactory* SceneFactory::getInstance() {
		if (nullptr == instance) {
			instance = new SceneFactory();
		}
		return instance;
	}
	
	void SceneFactory::destroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}
	void SceneFactory::initDefaultScene(int contextWidth, int contextHeight) {
		Scene::init(contextWidth, contextHeight);
		auto* scene = Scene::getInstance();

		//TODO

	}
}
