#include "MaterialFactory.h"
#include <algorithm>
#include "ShaderProgramFactory.h"
#include "VAO.h"
#include "VBO.h"
#include "MemoryManager.h"
#include "GeometryFactory.h"
#include "GlobalInclude.h"
#include "FBO.h"
#include <filesystem>

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

	Material* MaterialFactory::getPBRMaterial(const std::string& materialName) {
		if (const auto& iter = loadedPBRMaterials.find(materialName); iter != loadedPBRMaterials.end()) {
			return iter->second;
		}
		auto albedoPath = std::filesystem::path(
			AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/").append(materialName).append("_albedo.png")
		);
		if (!std::filesystem::exists(albedoPath)) {
			albedoPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_albedo.jpg")
			);
		}
		Texture2D* albedoMap = Allocator::New<Texture2D>();
		albedoMap->Init(albedoPath, ALBEDO_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE, false);
		albedoMap->GenerateMipmap();

		bool flipY = false;
		// TODO flipY is currently not used!
		Texture2D* normalMap = Allocator::New<Texture2D>();
		auto normalPath = std::filesystem::path(
			AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/").append(materialName).append("_normal.png")
		);
		if (!std::filesystem::exists(normalPath)) {
			normalPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_normal-ogl.png")
			);
		}
		if (!std::filesystem::exists(normalPath)) {
			normalPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_normal-dx.png")
			);
			flipY = true;
		}
		if (!std::filesystem::exists(normalPath)) {
			normalPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_normal.jpg")
			);
		}
		if (!std::filesystem::exists(normalPath)) {
			normalPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_normal-ogl.jpg")
			);
		}
		if (!std::filesystem::exists(normalPath)) {
			normalPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_normal-dx.jpg")
			);
			flipY = true;
		}
		normalMap->Init(normalPath, NORMAL_MAP_UNIT, GL_RGB, GL_UNSIGNED_BYTE, false);
		normalMap->GenerateMipmap();
		Texture2D roughnessMap;
		auto roughnessPath = std::filesystem::path(
			AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/").append(materialName).append("_roughness.png")
		);
		if (!std::filesystem::exists(roughnessPath)) {
			roughnessPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_roughness.jpg")
			);
		}
		roughnessMap.Init(roughnessPath, 0, GL_RGB, GL_UNSIGNED_BYTE, false);
		Texture2D metallicMap;
		auto metallicPath = std::filesystem::path(
			AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/").append(materialName).append("_metallic.png")
		);
		if (!std::filesystem::exists(metallicPath)) {
			metallicPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_metallic.jpg")
			);
		}
		metallicMap.Init(metallicPath, 1, GL_RGB, GL_UNSIGNED_BYTE, false);
		Texture2D aoMap;
		auto aoPath = std::filesystem::path(
			AssetFolderPathManager::getInstance()->getTextureFolderPath()
			.append(materialName).append("/").append(materialName).append("_ao.png")
		);
		if (!std::filesystem::exists(aoPath)) {
			aoPath = std::filesystem::path(
				AssetFolderPathManager::getInstance()->getTextureFolderPath()
				.append(materialName).append("/").append(materialName).append("_ao.jpg")
			);
		}
		aoMap.Init(aoPath, 2, GL_RGB, GL_UNSIGNED_BYTE, false);

		// Combining roughness, metallic and AO into a single texture:
		auto& dim = albedoMap->GetDimensions();
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
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/fullScreenQuad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/roughnessMetallicAOCombination.frag")
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
			roughnessMetallicAO->GenerateMipmap();
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

	Material* MaterialFactory::getHomogenousPBRMaterial(glm::vec3 albedo, float roughness, float metallic, float ao) {
		auto material = Allocator::New<Material>();
		auto shader = ShaderProgramFactory::GetInstance()->GetDeferredPBRProgramForHomogenousMaterial();
		material->Init(shader);
		material->setAlbedo(albedo);
		material->setRoughness(roughness);
		material->setMetallic(metallic);
		material->setAO(ao);
		return material;
	}
	
	Material* MaterialFactory::getEmissiveMaterial(const char* materialName, const glm::vec3& color, const float intensity)
	{
		ShaderProgram* program = ShaderProgramFactory::GetInstance()->GetDeferredEmissiveMaterialProgram();
		auto* volumeMaterial = Allocator::New<Material>();
		volumeMaterial->Init(program);
		volumeMaterial->setAlbedo(color * intensity);
		volumeMaterial->SetAlphaBlend(false);
		return volumeMaterial;
	}

	void MaterialFactory::ForgetPointers()
	{
		loadedPBRMaterials.clear();
	}
	

}
