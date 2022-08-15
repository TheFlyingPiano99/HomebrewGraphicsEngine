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

		void RestartScene();

		void LoadScene(int sceneId);

		void Draw();

		Scene* getScene() {
			return currentScene;
		}

		void FrameBeginningControl();

		int ControlAndUpdate(float dt);

		void FrameEndingControl();

	private:
		SceneManager() = default;

		~SceneManager() {
			if (nullptr != currentScene) {
				delete currentScene;
			}
		}

		static SceneManager* instance;

		Scene* currentScene;
	};
}

