#include "MemoryManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetFolderPathManager.h"
#include "GUI.h"
#include "SceneEventManager.h"
#include "MaterialFactory.h"
#include "ShaderProgramFactory.h"
#include "SceneManager.h"
#include "SceneFactory.h"


std::vector<void*> Hogra::MemoryManager::heapAllocatedInstances = std::vector<void*>();

	/*
	* Needs to be called before closing program!
	*/
	void Hogra::MemoryManager::DeallocateAll() {
		GUI::DestroyInstance();
		AssetFolderPathManager::DestroyInstance();
		ControlActionManager::DestroyInstance();
		SceneEventManager::DestroyInstance();
		MaterialFactory::DestroyInstance();
		GeometryFactory::DestroyInstance();
		ShaderProgramFactory::DestroyInstance();
		SceneManager::DestroyInstance();
		SceneFactory::DestroyInstance();

		for (auto& instance : heapAllocatedInstances) {
			delete instance;
		}
		heapAllocatedInstances.clear();
	}

	void Hogra::MemoryManager::Deallocate(void* instance)
	{
		auto iter = std::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), instance);
		if (iter != heapAllocatedInstances.end()) {
			heapAllocatedInstances.erase(iter);
			delete instance;
		}
	}
