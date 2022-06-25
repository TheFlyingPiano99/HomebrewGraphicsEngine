#include "Material.h"
namespace hograengine {

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
		glUniform3f(glGetUniformLocation(program->ID, "material.diffuseColor"), diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(glGetUniformLocation(program->ID, "material.specularColor"), specularColor.r, specularColor.g, specularColor.b);
		glUniform3f(glGetUniformLocation(program->ID, "material.ambientColor"), ambientColor.r, ambientColor.g, ambientColor.b);
		glUniform1f(glGetUniformLocation(program->ID, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(program->ID, "material.reflectiveness"), reflectiveness);

		if (alphaBlend) {
			glEnable(GL_BLEND);
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
}