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
		currentScene = SceneFactory::getInstance()->CreateVoxelDemoScene(contextWidth, contextHeight);		
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
	void SceneManager::BeforePhysicsLoopUpdate()
	{
		if (nullptr != currentScene) {
			currentScene->BeforePhysicsLoopUpdate();
		}
	}

	void SceneManager::AfterPhysicsLoopUpdate()
	{
		if (nullptr != currentScene) {
			currentScene->AfterPhysicsLoopUpdate();
		}
	}

	int SceneManager::PhysicsUpdate(float dt)
	{
		currentScene->PhysicsUpdate(dt);
		SceneChange change = currentScene->GetSceneChange();
		switch (change.changeType)
		{
		case SceneChange::ChangeType::noChange:
			break;
		case SceneChange::ChangeType::quit:
			return -1;
		case SceneChange::ChangeType::restartScene:
			RestartScene();
			return 0;
			break;
		case SceneChange::ChangeType::nextScene:
			LoadScene(change.nextSceneId);
			return 0;
			break;
		default:
			break;
		}
		return 0;
	}
}