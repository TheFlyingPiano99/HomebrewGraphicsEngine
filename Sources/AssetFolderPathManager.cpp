#include "AssetFolderPathManager.h"
#include <fstream>
#include <filesystem>
#include "DebugUtils.h"

namespace Hogra {

	AssetFolderPathManager* AssetFolderPathManager::instance = nullptr;

	std::filesystem::path AssetFolderPathManager::FindPathIntoFolder(const std::string& folderName) {
		std::string path = folderName;
		path.append("/");
		std::filesystem::path p = std::filesystem::path(folderName.c_str());
		int i = 0;
		while (!std::filesystem::is_directory(p)) {
			if (i > 5) {
				std::string errorMessage = "Folder \"";
				errorMessage.append(folderName);
				errorMessage.append("\" not found!");
				DebugUtils::PrintError("AssetFolderPathManager", path.c_str());
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

	AssetFolderPathManager* AssetFolderPathManager::getInstance() {
		if (instance == nullptr) {
			instance = Allocator::New<AssetFolderPathManager>();
		}
		return instance;
	}

	std::filesystem::path AssetFolderPathManager::getShaderFolderPath() {
		if (shaderFolderPath.empty()) {
			shaderFolderPath = FindPathIntoFolder("Resources/Shaders");
		}
		return shaderFolderPath;
	}

	std::filesystem::path AssetFolderPathManager::getComputeShaderFolderPath()
	{
		if (computeShaderFolderPath.empty()) {
			computeShaderFolderPath = FindPathIntoFolder("Resources/Shaders/Compute");
		}
		return computeShaderFolderPath;
	}

	std::filesystem::path AssetFolderPathManager::getTextureFolderPath() {
		if (textureFolderPath.empty()) {
			textureFolderPath = FindPathIntoFolder("Resources/Textures");
		}
		return textureFolderPath;
	}

	std::filesystem::path AssetFolderPathManager::getGeometryFolderPath()
	{
		if (geometryFolderPath.empty()) {
			geometryFolderPath = FindPathIntoFolder("Resources/Geometries");
		}
		return geometryFolderPath;
	}

	std::filesystem::path AssetFolderPathManager::getSavesFolderPath() {
		if (savesFolderPath.empty()) {
			savesFolderPath = FindPathIntoFolder("Saves");
		}
		return savesFolderPath;
	}
	std::filesystem::path AssetFolderPathManager::getFontsFolderPath()
	{
		if (fontsFolderPath.empty()) {
			fontsFolderPath = FindPathIntoFolder("Resources/Fonts");
		}
		return fontsFolderPath;
	}
	std::filesystem::path AssetFolderPathManager::getSoundsFolderPath()
	{
		if (soundsFolderPath.empty()) {
			soundsFolderPath = FindPathIntoFolder("Resources/Sounds");
		}
		return soundsFolderPath;
	}
	std::filesystem::path AssetFolderPathManager::getIconsFolderPath()
	{
		if (iconsFolderPath.empty()) {
			iconsFolderPath = FindPathIntoFolder("Resources/Icons");
		}
		return iconsFolderPath;
	}
	std::filesystem::path AssetFolderPathManager::getScenesFolderPath()
	{
		if (scenesFolderPath.empty()) {
			scenesFolderPath = FindPathIntoFolder("Resources/Scenes");
		}
		return scenesFolderPath;
	}
}