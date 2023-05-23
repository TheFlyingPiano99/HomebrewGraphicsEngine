#pragma once

#include <string>
#include "MemoryManager.h"

namespace Hogra {

	/*
	* Singleton
	*/
	class AssetFolderPathManager {
		ALLOCATOR_CONSTRUCTIBLE
		
		static AssetFolderPathManager* instance;

		std::string shaderFolderPath;
		std::string computeShaderFolderPath;
		std::string textureFolderPath;
		std::string geometryFolderPath;
		std::string fontsFolderPath;
		std::string savesFolderPath;
		std::string soundsFolderPath;
		std::string iconsFolderPath;
		std::string scenesFolderPath;


		class AssetFolderNotFound : public std::exception {
		public:
			explicit AssetFolderNotFound(const char* message) : std::exception(message) {

			}
		};

		std::string FindPathIntoFolder(std::string folderName);


	public:

		static AssetFolderPathManager* getInstance() {
			if (instance == nullptr) {
				instance = Allocator::New<AssetFolderPathManager>();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator::Delete(instance);
		}

		std::string getShaderFolderPath();

		std::string getComputeShaderFolderPath();

		std::string getTextureFolderPath();

		std::string getGeometryFolderPath();

		std::string getSavesFolderPath();

		std::string getFontsFolderPath();

		std::string getSoundsFolderPath();

		std::string getIconsFolderPath();

		std::string getScenesFolderPath();

	};
}