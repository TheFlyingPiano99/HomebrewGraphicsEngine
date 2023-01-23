#include "MaterialFactory.h"
#include <algorithm>
#include "ShaderProgramFactory.h"
#include "VAO.h"
#include "VBO.h"
#include "MemoryManager.h"

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

		const auto& iter = loadedPBRMaterials.find(materialName);
		if (iter != loadedPBRMaterials.end()) {
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
		VAO vao;
		VBO vbo;
		vao.Init();
		vao.Bind();
		vao.Bind();
		std::vector<glm::vec4> vertices;
		vertices.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));	//6
		vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//5
		vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //4
		vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//3
		vertices.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f));//2
		vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //1
		vbo.Init(vertices);
		vao.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);

		FBO fbo;
		fbo.Init();
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *roughnessMetallicAO);
		fbo.Bind();
		ShaderProgram combinationProgram;
		combinationProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("roughnessMetallicAOCombination.frag")
			);
		combinationProgram.Activate();
		roughnessMap.Bind();
		metallicMap.Bind();
		aoMap.Bind();
		glm::mat4 projection(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(combinationProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		vao.Unbind();
		fbo.Unbind();
		roughnessMap.Unbind();
		metallicMap.Unbind();
		aoMap.Unbind();

		auto* shader = ShaderProgramFactory::GetInstance()->GetDefaultPBRProgramWithMapping();
		auto* material = Allocator::New<Material>();
		material->Init(shader);
		material->addTexture(albedoMap);
		material->addTexture(normalMap);
		material->addTexture(roughnessMetallicAO);
		loadedPBRMaterials.emplace(materialName, material);
		return material;
	}
	
	Material* MaterialFactory::getEmissiveMaterial(const char* materialName, const glm::vec3& color, const float intensity)
	{
		ShaderProgram* program = ShaderProgramFactory::GetInstance()->GetDeferredEmissiveMaterialProgram();
		auto* material = Allocator::New<Material>();
		material->Init(program);
		material->setAlbedo(color * intensity);
		material->setAlphaBlend(false);
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
