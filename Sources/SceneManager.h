#pragma once
#include "Scene.h"
#include "MemoryManager.h"

namespace Hogra {

    class SceneManager
    {
        ALLOCATOR_CONSTRUCTIBLE

    public:
        SceneManager() = default;

        ~SceneManager() {
            UnloadCurrentScene();
        }

        static SceneManager* getInstance();

        static void DestroyInstance() {
            if (nullptr != instance) {
                Allocator::Delete(instance);
                instance = nullptr;
            }
        }

        void InitSplash(int contextWidth, int contextHeight);

        void Init(int contextWidth, int contextHeight);

        void RestartScene();

        void LoadScene(int sceneId);

        void UnloadCurrentScene();

        void Draw();

        Scene* getScene() {
            return currentScene;
        }

        void BeforePhysicsLoopUpdate();

        int PhysicsUpdate(float dt_sec);

        void AfterPhysicsLoopUpdate();

        void OnWindowResize(unsigned int w, unsigned int h);

    private:

        static SceneManager* instance;

        Scene* currentScene;
    };
}

