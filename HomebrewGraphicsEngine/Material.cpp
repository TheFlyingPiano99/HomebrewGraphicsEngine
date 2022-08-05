#include "Material.h"
#include "MemoryManager.h"
#include <iostream>

namespace Hogra {

	std::vector<Material*> Material::heapAllocatedInstances = std::vector<Material*>();

	Material* Material::Instantiate()
	{
		auto* instance = new Material();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}

	void Material::Init(ShaderProgram* program)
	{
		this->program = program;
	}
	void Material::Bind() const
	{
		if (nullptr == program) {
			throw ShaderProgramIsNullptr();
		}
		program->Activate();
		for (auto& texture : textures)
		{
			texture->Bind();
		}
		glUniform3f(glGetUniformLocation(program->ID, "material.albedo"), albedo.r, albedo.g, albedo.b);
		glUniform1f(glGetUniformLocation(program->ID, "material.roughness"), roughness);
		glUniform1f(glGetUniformLocation(program->ID, "material.metallic"), metallic);
		glUniform1f(glGetUniformLocation(program->ID, "material.ao"), ao);

		if (alphaBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(srcBlendFunc, destBlendFunc);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	void Material::addTexture(Texture* texture) { textures.push_back(texture); }

	void Material::clearTextures()
	{
		textures.clear();
	}

	ShaderProgram* Material::getShaderProgram() const
	{
		return program;
	}

	std::span<Texture*> Material::getTextures()
	{
		return textures;
	}

	void Material::setAlphaBlend(bool blend) {
		alphaBlend = blend;
	}
	
	void Material::setBlendFunc(int src, int dest) {
		srcBlendFunc = src;
		destBlendFunc = dest;
	}
}