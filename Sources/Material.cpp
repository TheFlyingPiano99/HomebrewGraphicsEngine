#include "Material.h"
#include "MemoryManager.h"
#include <iostream>

namespace Hogra {

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

		program->SetUniform("material.albedo", albedo);
		program->SetUniform("material.roughness", roughness);
		program->SetUniform("material.metallic", metallic);
		program->SetUniform("material.ao", ao);

		if (alphaBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(srcBlendFunc, destBlendFunc);
		}
		else {
			glDisable(GL_BLEND);
		}
	}

	void Material::AddTexture(Texture* texture) { textures.push_back(texture); }

	void Material::ClearTextures()
	{
		textures.clear();
	}

	ShaderProgram* Material::GetShaderProgram() const
	{
		return program;
	}

	std::span<Texture*> Material::GetTextures()
	{
		return textures;
	}

	void Material::SetAlphaBlend(bool blend) {
		alphaBlend = blend;
	}
	
	void Material::SetBlendFunc(int src, int dest) {
		srcBlendFunc = src;
		destBlendFunc = dest;
	}
}