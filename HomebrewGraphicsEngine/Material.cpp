#include "Material.h"
#include <iostream>

namespace Hogra {

	int Material::instanceCount = 0;

	Material* Material::Instantiate()
	{
		instanceCount++;
		return new Material();
	}

	void Material::Deallocate(Material* instance)
	{
		instanceCount--;
		delete instance;
	}

	void Material::PrintInstanceCount()
	{
		std::cout << "Material instance count: " << instanceCount << std::endl;
	}

	void Material::Init(ShaderProgram* program)
	{
		this->program = program;
	}
	void Material::Bind() const
	{
		if (nullptr == program) {
			throw new ShaderProgramIsNullptr();
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

	void Material::addTexture(const Texture* texture) { textures.push_back(texture); }

	void Material::clearTextures()
	{
		textures.clear();
	}

	ShaderProgram* Material::getShaderProgram() const
	{
		return program;
	}

	std::span<const Texture* const> Material::getTextures() const
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