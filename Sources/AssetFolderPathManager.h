#pragma once

#include <string>
#include "MemoryManager.h"
#include <filesystem>

namespace Hogra {

	/*
	* Singleton
	*/
	class AssetFolderPathManager {
		ALLOCATOR_CONSTRUCTIBLE
		
		static AssetFolderPathManager* instance;

		std::filesystem::path shaderFolderPath;
		std::filesystem::path computeShaderFolderPath;
		std::filesystem::path textureFolderPath;
		std::filesystem::path geometryFolderPath;
		std::filesystem::path fontsFolderPath;
		std::filesystem::path savesFolderPath;
		std::filesystem::path soundsFolderPath;
		std::filesystem::path iconsFolderPath;
		std::filesystem::path scenesFolderPath;


		class AssetFolderNotFound : public std::exception {
		public:
			explicit AssetFolderNotFound(const char* message) : std::exception(message) {

			}
		};

		std::filesystem::path FindPathIntoFolder(const std::string& folderName);


	public:

		static AssetFolderPathManager* getInstance();

		static void DestroyInstance() {
			Allocator::Delete(instance);
		}

		std::filesystem::path getShaderFolderPath();
		std::filesystem::path getComputeShaderFolderPath();
		std::filesystem::path getTextureFolderPath();
		std::filesystem::path getGeometryFolderPath();
		std::filesystem::path getSavesFolderPath();
		std::filesystem::path getFontsFolderPath();
		std::filesystem::path getSoundsFolderPath();
		std::filesystem::path getIconsFolderPath();
		std::filesystem::path getScenesFolderPath();

	};
}