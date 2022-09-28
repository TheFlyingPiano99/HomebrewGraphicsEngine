#pragma once
#include "Scene.h"
#include "MemoryManager.h"

namespace Hogra {

	class SceneManager
	{
		friend class Allocator<SceneManager>;
	public:

		static SceneManager* getInstance();

		static void DestroyInstance() {
			if (nullptr != instance) {
				Allocator<SceneManager>::Delete(instance);
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
			Allocator<Scene>::Delete(currentScene);
		}

		static SceneManager* instance;

		Scene* currentScene;
	};
}

