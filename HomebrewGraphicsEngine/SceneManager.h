#pragma once
#include "Scene.h"

namespace hograengine {

	class SceneManager
	{
	public:

		static SceneManager* getInstance();

		static void destroyInstance() {
			if (nullptr != instance) {
				delete instance;
				instance = nullptr;
			}
		}

		void init(int contextWidth, int contextHeight);

		void draw();

		Scene* getScene() {
			return Scene::getInstance();
		}

	private:
		SceneManager() = default;
		static SceneManager* instance;
	};
}

