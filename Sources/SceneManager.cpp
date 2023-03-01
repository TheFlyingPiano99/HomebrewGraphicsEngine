#include "SceneManager.h"
#include "SceneFactory.h"
#include "MemoryManager.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "nlohmann/json.hpp"


struct SceneIDToFilenameMapping {
public:
	SceneIDToFilenameMapping() {}
	SceneIDToFilenameMapping(std::map<int, std::string> const& s) : scenes(s) {
	}

	std::map<int, std::string> scenes;
};
#define INIT_MODE 4

namespace Hogra {
	SceneManager* SceneManager::instance = nullptr;

	SceneManager* SceneManager::getInstance() {
		if (nullptr == instance) {
			instance = Allocator::New<SceneManager>();
		}
		return instance;
	}

	void SceneManager::Init(int contextWidth, int contextHeight)
	{
		if (nullptr != currentScene) {
			return;
		}
#if 0 == INIT_MODE
#elif 1 == INIT_MODE
		currentScene = SceneFactory::getInstance()->CreatePixelPhysicsDemoScene(contextWidth, contextHeight);
#elif 2 == INIT_MODE
		currentScene = SceneFactory::getInstance()->CreateDemoScene(contextWidth, contextHeight);
#elif 3 == INIT_MODE
		currentScene = SceneFactory::getInstance()->CreateEasyScene(contextWidth, contextHeight);
#elif 4 == INIT_MODE
		LoadScene(2);		// Magic mirror
#endif

	}

	void SceneManager::RestartScene()
	{
		if (nullptr == currentScene) {
			return;
		}
		Allocator::Delete(currentScene);
		ControlActionManager::getInstance()->UnregisterControls();
		currentScene = SceneFactory::getInstance()->CreateDemoScene(GlobalVariables::windowWidth, GlobalVariables::windowHeight);
	}

	void SceneManager::LoadScene(int sceneId)
	{
		auto mappingFile = std::fstream(AssetFolderPathManager::getInstance()->getScenesFolderPath().append("sceneIdToFilenameMapping.json"));
		if (mappingFile.is_open()) {
			nlohmann::json jsonData = nlohmann::json::parse(mappingFile);		// Read from file
			mappingFile.close();

			auto& jsonScenes = jsonData["scenes"];
			auto s = jsonScenes.size();
			std::string filename;
			for (auto i = 0; i < s; i++) {
				if (sceneId == jsonScenes[i]["id"]) {
					filename = jsonScenes[i]["filename"];
					break;
				}
			}

			if (filename.empty()) {
				std::cerr << "Scene manager error: Trying to load scene where the filename for the ID is not specified!" << std::endl;
				return;
			}
			UnloadCurrentScene();
			currentScene = SceneFactory::getInstance()->LoadSceneFromFile(AssetFolderPathManager::getInstance()->getScenesFolderPath().append(filename));
		}
		else {
			std::cerr << "Scene manager error: 'Scene ID to filename mapping' file not found!" << std::endl;
		}
	}

	void SceneManager::UnloadCurrentScene()
	{
		if (currentScene == nullptr) {
			return;
		}
		currentScene->Serialize();
		Allocator::Delete(currentScene);
	}

	void SceneManager::Draw()
	{
		if (nullptr != currentScene) {
			currentScene->Draw();
		}
	}

	void SceneManager::BeforePhysicsLoopUpdate()
	{
		if (nullptr != currentScene) {
			currentScene->BeforePhysicsLoopUpdate();
		}
	}

	int SceneManager::PhysicsUpdate(float dt)
	{
		if (nullptr == currentScene) {
			return 0;
		}
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

	void SceneManager::AfterPhysicsLoopUpdate()
	{
		if (nullptr != currentScene) {
			currentScene->AfterPhysicsLoopUpdate();
		}
	}

	void SceneManager::OnWindowResize(unsigned int w, unsigned int h)
	{
		if (nullptr == currentScene) {
			return;
		}
		currentScene->OnContextResize(w, h);
	}

}