#pragma once

#include <string>

namespace hograengine {

	/*
	* Singleton
	*/
	class AssetFolderPathManager {
		static AssetFolderPathManager* instance;

		std::string shaderFolderPath;
		std::string textureFolderPath;
		std::string geometryFolderPath;
		std::string savesFolderPath;

		class AssetFolderNotFound : public std::exception {
		public:
			explicit AssetFolderNotFound(const char* message) : std::exception(message) {

			}
		};

		AssetFolderPathManager() = default;

		~AssetFolderPathManager() = default;

		std::string findPathIntoFolder(std::string folderName);


	public:

		static AssetFolderPathManager* getInstance() {
			if (instance == nullptr) {
				instance = new AssetFolderPathManager();
			}
			return instance;
		}

		static void destroyInstance() {
			if (instance != nullptr) {
				delete instance;
				instance = nullptr;
			}
		}

		std::string getShaderFolderPath();

		std::string getTextureFolderPath();

		std::string getGeometryFolderPath();

		std::string getSavesFolderPath();
	};
}