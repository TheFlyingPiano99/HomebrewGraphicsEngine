#include "MaterialFactory.h"
#include <algorithm>
#include "ShaderProgramFactory.h"
#include "VAO.h"
#include "VBO.h"
#include "MemoryManager.h"
#include "GeometryFactory.h"

namespace Hogra {

	MaterialFactory* MaterialFactory::instance = nullptr;

	MaterialFactory* MaterialFactory::GetInstance()
	{
		if (nullptr == instance) {
			instance = Allocator::New<MaterialFactory>();
		}
		return instance;
	}

	void MaterialFactory::DestroyInstance()
	{
		Allocator::Delete(instance);
	}

	Material* MaterialFactory::getPBRMaterial(const char* materialName) {

		if (const auto& iter = loadedPBRMaterials.find(materialName); iter != loadedPBRMaterials.end()) {
			return iter->second;
		}
		Texture2D* albedoMap = Allocator::New<Texture2D>();
		albedoMap->Init(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/albedo.jpg"), ALBEDO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		Texture2D* normalMap = Allocator::New<Texture2D>();
		normalMap->Init(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/normal.jpg"), NORMAL_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);
		Texture2D roughnessMap;
		roughnessMap.Init(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/roughness.jpg"), 0, GL_RGB, GL_UNSIGNED_BYTE);
		Texture2D metallicMap;
		metallicMap.Init(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/metallic.jpg"), 1, GL_RGB, GL_UNSIGNED_BYTE);
		Texture2D aoMap;
		aoMap.Init(AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/ao.jpg"), 2, GL_RGB, GL_UNSIGNED_BYTE);

		// Combining roughness, metallic and AO into a single texture:
		auto& dim = albedoMap->getDimensions();
		Texture2D* roughnessMetallicAO = Allocator::New<Texture2D>();
		roughnessMetallicAO->Init(GL_RGBA, dim, ROUGHNESS_METALLIC_AO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE);

		// Combine maps:
		{
			auto quad = GeometryFactory::GetInstance()->GetSimpleQuad();
			FBO fbo;
			fbo.Init();
			fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *roughnessMetallicAO);
			fbo.Bind();
			ShaderProgram combinationProgram;
			combinationProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("quad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("roughnessMetallicAOCombination.frag")
			);
			combinationProgram.Activate();
			roughnessMap.Bind();
			metallicMap.Bind();
			aoMap.Bind();
			glm::mat4 projection(1.0f);
			combinationProgram.SetUniform("projection", projection);
			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			quad->Draw();
			fbo.Unbind();
			roughnessMap.Unbind();
			metallicMap.Unbind();
			aoMap.Unbind();
		}

		auto* shader = ShaderProgramFactory::GetInstance()->GetDeferredPBRProgramWithMapping();
		auto* material = Allocator::New<Material>();
		material->Init(shader);
		material->AddTexture(albedoMap);
		material->AddTexture(normalMap);
		material->AddTexture(roughnessMetallicAO);
		loadedPBRMaterials.emplace(materialName, material);
		return material;
	}
	
	Material* MaterialFactory::getEmissiveMaterial(const char* materialName, const glm::vec3& color, const float intensity)
	{
		ShaderProgram* program = ShaderProgramFactory::GetInstance()->GetDeferredEmissiveMaterialProgram();
		auto* material = Allocator::New<Material>();
		material->Init(program);
		material->setAlbedo(color * intensity);
		material->SetAlphaBlend(false);
		return material;
	}

	void MaterialFactory::ForgetPointers()
	{
		loadedPBRMaterials.clear();
	}
	
	MaterialFactory::~MaterialFactory()
	{

	}
}
