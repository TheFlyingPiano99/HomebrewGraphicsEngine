#include "SceneManager.h"
#include "SceneFactory.h"

namespace Hogra {
	SceneManager* SceneManager::instance = nullptr;

	SceneManager* SceneManager::getInstance() {
		if (nullptr == instance) {
			instance = new SceneManager();
		}
		return instance;
	}
	void SceneManager::Init(int contextWidth, int contextHeight)
	{
		if (nullptr != currentScene) {
			return;
		}
		currentScene = SceneFactory::getInstance()->CreateDemoScene(contextWidth, contextHeight);		
	}
	void SceneManager::Draw()
	{
		currentScene->Draw();
	}
	void SceneManager::UpdateAndControl(float dt)
	{
		currentScene->Control(dt);
		currentScene->Update(dt);
	}
}