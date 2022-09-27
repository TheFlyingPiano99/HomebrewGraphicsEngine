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
#include "AudioBuffer.h"
#include "AudioSource.h"
#include "AudioListener.h"


namespace Hogra {
	/*
	* Needs to be called before closing program!
	*/
	void MemoryManager::DeallocateAll() {
		GUI::DestroyInstance();
		AssetFolderPathManager::DestroyInstance();
		ControlActionManager::DestroyInstance();
		SceneEventManager::DestroyInstance();
		MaterialFactory::DestroyInstance();
		GeometryFactory::DestroyInstance();
		ShaderProgramFactory::DestroyInstance();
		SceneManager::DestroyInstance();
		SceneFactory::DestroyInstance();

		ShaderProgram::DeallocateAll();
		Material::DeallocateAll();
		Geometry::DeallocateAll();
		SceneObject::DeallocateAll();
		Texture::DeallocateAll();
		Component::DeallocateAll();
		Font::DeallocateAll();

		AudioBuffer::DeallocateAll();
		AudioSource::DeallocateAll();
		AudioListener::DeallocateAll();
	}

	void MemoryManager::DeallocateSceneResources()
	{
		ShaderProgram::DeallocateAll();
		Material::DeallocateAll();
		Geometry::DeallocateAll();
		SceneObject::DeallocateAll();
		Texture::DeallocateAll();
		Component::DeallocateAll();
		Font::DeallocateAll();

		AudioBuffer::DeallocateAll();
		AudioSource::DeallocateAll();
		AudioListener::DeallocateAll();

		MaterialFactory::GetInstance()->ForgetPointers();
		GeometryFactory::GetInstance()->ForgetPointers();
		ShaderProgramFactory::GetInstance()->ForgetPointers();
	}

	std::vector<std::function<void()>> MasterAllocator::deleteAllFunctions = std::vector<std::function<void()>>();

}

