#pragma once
#include "Scene.h"
#include "MemoryManager.h"

namespace Hogra {

	class SceneManager
	{
		friend class Allocator;
	public:

		static SceneManager* getInstance();

		static void DestroyInstance() {
			if (nullptr != instance) {
				Allocator::Delete(instance);
				instance = nullptr;
			}
		}

		void Init(int contextWidth, int contextHeight, int argc = 0, char* argv[] = nullptr);

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

		void UnloadScene();

	private:
		SceneManager() = default;

		~SceneManager() {
			UnloadScene();
		}

		static SceneManager* instance;

		Scene* currentScene;
	};
}

