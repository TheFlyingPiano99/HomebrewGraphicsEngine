#pragma once

#include <string>
#include "MemoryManager.h"

namespace Hogra {

	/*
	* Singleton
	*/
	class AssetFolderPathManager {
		friend class Allocator<AssetFolderPathManager>;
		static AssetFolderPathManager* instance;

		std::string shaderFolderPath;
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

		AssetFolderPathManager() = default;

		~AssetFolderPathManager() = default;

		std::string FindPathIntoFolder(std::string folderName);


	public:

		static AssetFolderPathManager* getInstance() {
			if (instance == nullptr) {
				instance = Allocator<AssetFolderPathManager>::New();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator<AssetFolderPathManager>::Delete(instance);
		}

		std::string getShaderFolderPath();

		std::string getTextureFolderPath();

		std::string getGeometryFolderPath();

		std::string getSavesFolderPath();

		std::string getFontsFolderPath();

		std::string getSoundsFolderPath();

		std::string getIconsFolderPath();

		std::string getScenesFolderPath();

	};
}