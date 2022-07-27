#pragma once
#include "Scene.h"

namespace Hogra {

	class SceneManager
	{
	public:

		static SceneManager* getInstance();

		static void DestroyInstance() {
			if (nullptr != instance) {
				delete instance;
				instance = nullptr;
			}
		}

		void Init(int contextWidth, int contextHeight);

		void Draw();

		Scene* getScene() {
			return currentScene;
		}

		void UpdateAndControl(float dt);

	private:
		SceneManager() = default;
		static SceneManager* instance;

		Scene* currentScene;
	};
}

