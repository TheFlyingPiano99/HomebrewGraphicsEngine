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

		void Init(int contextWidth, int contextHeight);

		void RestartScene();

		void LoadScene(int sceneId);

		void UnloadCurrentScene();

		void Draw();

		Scene* getScene() {
			return currentScene;
		}

		void BeforePhysicsLoopUpdate();

		int PhysicsUpdate(float dt);

		void AfterPhysicsLoopUpdate();

		void OnWindowResize(unsigned int w, unsigned int h);

	private:
		SceneManager() = default;

		~SceneManager() {
			UnloadCurrentScene();
		}

		static SceneManager* instance;

		Scene* currentScene;
	};
}

