#include "AssetFolderPathManager.h"
#include <fstream>
#include <filesystem>
namespace hograengine {

	AssetFolderPath* AssetFolderPath::instance = nullptr;

	inline std::string AssetFolderPath::findPathIntoFolder(std::string folderName) {
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

	std::string AssetFolderPath::getShaderFolderPath() {
		if (shaderFolderPath.empty()) {
			shaderFolderPath = findPathIntoFolder("Resources/Shaders");
		}
		return shaderFolderPath;
	}

	std::string AssetFolderPath::getTextureFolderPath() {
		if (textureFolderPath.empty()) {
			textureFolderPath = findPathIntoFolder("Resources/Textures");
		}
		return textureFolderPath;
	}

	std::string AssetFolderPath::getGeometryFolderPath()
	{
		if (geometryFolderPath.empty()) {
			geometryFolderPath = findPathIntoFolder("Resources/Geometries");
		}
		return geometryFolderPath;
	}

	std::string AssetFolderPath::getSavesFolderPath() {
		if (savesFolderPath.empty()) {
			savesFolderPath = findPathIntoFolder("Saves");
		}
		return savesFolderPath;
	}
}