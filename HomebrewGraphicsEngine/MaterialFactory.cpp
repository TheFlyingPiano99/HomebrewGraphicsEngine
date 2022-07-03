#include "MaterialFactory.h"
#include <algorithm>
#include "ShaderProgramFactory.h"

namespace hograengine {

	MaterialFactory* MaterialFactory::instance = nullptr;

	MaterialFactory* MaterialFactory::getInstance()
	{
		if (nullptr == instance) {
			instance = new MaterialFactory();
		}
		return instance;
	}

	void MaterialFactory::destroyInstance()
	{
		if (nullptr != instance) {
			delete instance;
		}
	}

	Material* MaterialFactory::getPBRMaterial(const char* materialName, const Texture* skyBox) {

		const auto& iter = loadedPBRMaterials.find(materialName);
		if (iter != loadedPBRMaterials.end()) {
			return iter->second;
		}
		const auto* albedoMap = new Texture2D(AssetFolderPathManager::getInstance()->getTextureFolderPath()
		.append(materialName).append("/albedo.jpg"), ALBEDO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* normalMap = new Texture2D(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/normal.jpg"), NORMAL_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* roughnessMap = new Texture2D(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/roughness.jpg"), ROUGHNESS_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* metallicMap = new Texture2D(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/metallic.jpg"), METALLIC_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* aoMap = new Texture2D(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/ao.jpg"), AO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		auto* shader = ShaderProgramFactory::getInstance()->getDefaultPBRProgramWithMapping();
		auto* material = new Material(shader);
		material->addTexture(albedoMap);
		material->addTexture(normalMap);
		material->addTexture(roughnessMap);
		material->addTexture(metallicMap);
		material->addTexture(aoMap);
		material->addTexture(skyBox);
		loadedPBRMaterials.emplace(materialName, material);
		return material;
	}
	MaterialFactory::~MaterialFactory()
	{

	}
}
