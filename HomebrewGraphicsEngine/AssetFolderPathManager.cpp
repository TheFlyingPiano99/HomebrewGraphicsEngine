#include "AssetFolderPathManager.h"
#include <fstream>
#include <filesystem>
namespace hograengine {

	AssetFolderPathManager* AssetFolderPathManager::instance = nullptr;

	inline std::string AssetFolderPathManager::findPathIntoFolder(std::string folderName) {
		std::string path = folderName;
		path.append("/");
		std::filesystem::path p = std::filesystem::path(folderName.c_str());
		int i = 0;
		while (!std::filesystem::is_directory(p)) {
			if (i > 5) {
				std::string errorMessage = "Folder <";
				errorMessage.append(folderName);
				errorMessage.append("> not found!");
				throw new AssetFolderNotFound(errorMessage.c_str());
			}
			path.clear();
			for (int j = 0; j < i; j++) {
				path = path.append("../");
			}
			path = path.append(folderName);
			path.append("/");
			p = std::filesystem::path(path.c_str());
			i++;
		}
		return path;
	}

	std::string AssetFolderPathManager::getShaderFolderPath() {
		if (shaderFolderPath.empty()) {
			shaderFolderPath = findPathIntoFolder("Resources/Shaders");
		}
		return shaderFolderPath;
	}

	std::string AssetFolderPathManager::getTextureFolderPath() {
		if (textureFolderPath.empty()) {
			textureFolderPath = findPathIntoFolder("Resources/Textures");
		}
		return textureFolderPath;
	}

	std::string AssetFolderPathManager::getGeometryFolderPath()
	{
		if (geometryFolderPath.empty()) {
			geometryFolderPath = findPathIntoFolder("Resources/Geometries");
		}
		return geometryFolderPath;
	}

	std::string AssetFolderPathManager::getSavesFolderPath() {
		if (savesFolderPath.empty()) {
			savesFolderPath = findPathIntoFolder("Saves");
		}
		return savesFolderPath;
	}
}