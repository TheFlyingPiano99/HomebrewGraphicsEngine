#include "AssetFolderPathManager.h"
#include <fstream>
#include <filesystem>
namespace Hogra {

	AssetFolderPathManager* AssetFolderPathManager::instance = nullptr;

	inline std::string AssetFolderPathManager::FindPathIntoFolder(std::string folderName) {
		std::string path = folderName;
		path.append("/");
		std::filesystem::path p = std::filesystem::path(folderName.c_str());
		int i = 0;
		while (!std::filesystem::is_directory(p)) {
			if (i > 5) {
				std::string errorMessage = "Folder <";
				errorMessage.append(folderName);
				errorMessage.append("> not found!");
				throw AssetFolderNotFound(errorMessage.c_str());
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
			shaderFolderPath = FindPathIntoFolder("Resources/Shaders");
		}
		return shaderFolderPath;
	}

	std::string AssetFolderPathManager::getTextureFolderPath() {
		if (textureFolderPath.empty()) {
			textureFolderPath = FindPathIntoFolder("Resources/Textures");
		}
		return textureFolderPath;
	}

	std::string AssetFolderPathManager::getGeometryFolderPath()
	{
		if (geometryFolderPath.empty()) {
			geometryFolderPath = FindPathIntoFolder("Resources/Geometries");
		}
		return geometryFolderPath;
	}

	std::string AssetFolderPathManager::getSavesFolderPath() {
		if (savesFolderPath.empty()) {
			savesFolderPath = FindPathIntoFolder("Saves");
		}
		return savesFolderPath;
	}
	std::string AssetFolderPathManager::getFontsFolderPath()
	{
		if (fontsFolderPath.empty()) {
			fontsFolderPath = FindPathIntoFolder("Resources/Fonts");
		}
		return fontsFolderPath;
	}
	std::string AssetFolderPathManager::getSoundsFolderPath()
	{
		if (soundsFolderPath.empty()) {
			soundsFolderPath = FindPathIntoFolder("Resources/Sounds");
		}
		return soundsFolderPath;
	}
	std::string AssetFolderPathManager::getIconsFolderPath()
	{
		if (iconsFolderPath.empty()) {
			iconsFolderPath = FindPathIntoFolder("Resources/Icons");
		}
		return iconsFolderPath;
	}
	std::string AssetFolderPathManager::getScenesFolderPath()
	{
		if (scenesFolderPath.empty()) {
			scenesFolderPath = FindPathIntoFolder("Resources/Scenes");
		}
		return scenesFolderPath;
	}
}