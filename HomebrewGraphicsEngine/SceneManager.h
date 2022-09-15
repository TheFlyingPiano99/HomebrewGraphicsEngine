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

		void BeforePhysicsLoopUpdate();

		int PhysicsUpdate(float dt);

		void AfterPhysicsLoopUpdate();

		void HandleControlActions();

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

