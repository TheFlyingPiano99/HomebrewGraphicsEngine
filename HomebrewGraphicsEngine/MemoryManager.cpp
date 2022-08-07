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

		ShaderProgram::DeallocateAll();
		Material::DeallocateAll();
		Geometry::DeallocateAll();
		SceneObject::DeallocateAll();
		Texture::DeallocateAll();
		Component::DeallocateAll();
		Font::DeallocateAll();
	}

	void Hogra::MemoryManager::DeallocateSceneResources()
	{
		ShaderProgram::DeallocateAll();
		Material::DeallocateAll();
		Geometry::DeallocateAll();
		SceneObject::DeallocateAll();
		Texture::DeallocateAll();
		Component::DeallocateAll();
		Font::DeallocateAll();

		MaterialFactory::GetInstance()->ForgetPointers();
		GeometryFactory::GetInstance()->ForgetPointers();
		ShaderProgramFactory::GetInstance()->ForgetPointers();
	}

