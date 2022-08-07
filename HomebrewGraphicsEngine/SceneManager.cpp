#include "SceneManager.h"
#include "SceneFactory.h"
#include "MemoryManager.h"
#include "GlobalInclude.h"

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

	void SceneManager::RestartScene()
	{
		if (nullptr == currentScene) {
			return;
		}
		delete currentScene;
		MemoryManager::DeallocateSceneResources();
		currentScene = SceneFactory::getInstance()->CreateDemoScene(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight);
	}

	void SceneManager::LoadScene(int sceneId)
	{
		//TODO
	}

	void SceneManager::Draw()
	{
		currentScene->Draw();
	}
	int SceneManager::ControlAndUpdate(float dt)
	{
		currentScene->Control(dt);
		SceneChangeEvent change = currentScene->GetNextScene();
		switch (change.changeType)
		{
		case SceneChangeEvent::ChangeType::noChange:
			break;
		case SceneChangeEvent::ChangeType::quit:
			return -1;
		case SceneChangeEvent::ChangeType::restartScene:
			RestartScene();
			return 0;
			break;
		case SceneChangeEvent::ChangeType::nextScene:
			LoadScene(change.nextSceneId);
			return 0;
			break;
		default:
			break;
		}
		currentScene->Update(dt);
		return 0;
	}
}