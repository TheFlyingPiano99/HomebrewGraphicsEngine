#include "MaterialFactory.h"

namespace hograengine {
	Material* hograengine::MaterialFactory::createPBR(const char* materialName, const Texture* skyBox) {
		const auto* albedoMap = new Texture2D(AssetFolderPath::getInstance()->getTextureFolderPath()
		.append(materialName).append("/albedo.jpg"), ALBEDO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* normalMap = new Texture2D(AssetFolderPath::getInstance()->getTextureFolderPath()
			.append(materialName).append("/normal.jpg"), NORMAL_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* metallicMap = new Texture2D(AssetFolderPath::getInstance()->getTextureFolderPath()
			.append(materialName).append("/metallic.jpg"), METALLIC_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* roughnessMap = new Texture2D(AssetFolderPath::getInstance()->getTextureFolderPath()
			.append(materialName).append("/roughness.jpg"), ROUGHNESS_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		const auto* aoMap = new Texture2D(AssetFolderPath::getInstance()->getTextureFolderPath()
			.append(materialName).append("/ao.jpg"), AO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		ShaderProgram* shader = new ShaderProgram(
			AssetFolderPath::getInstance()->getShaderFolderPath().append("default.vert"),
			AssetFolderPath::getInstance()->getShaderFolderPath().append("default.frag")
		);
		auto* material = new Material(shader);
		material->addTexture(albedoMap);
		material->addTexture(normalMap);
		material->addTexture(metallicMap);
		material->addTexture(roughnessMap);
		material->addTexture(aoMap);
		material->addTexture(skyBox);
		material->setReflectiveness(0.3f);
		return material;
	}
}
