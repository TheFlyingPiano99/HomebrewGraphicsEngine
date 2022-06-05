#pragma once
#include <vector>
#include "Texture2D.h"
#include "Camera.h"
#include "Light.h"

class Material
{
public:
	Material(ShaderProgram* program) : program(program) {};

	void Bind() const;

	void Bind(const Camera& camera, const std::vector<Light*>& lights) const;

	void addTexture(Texture* texture);
	void clearTextures();

	ShaderProgram* getShaderProgram() const;

	const std::vector<Texture*>& getTextures() const;

	void setDiffuseColor(glm::vec3 color) {
		this->diffuseColor = color;
	}

	void setSpecularColor(glm::vec3 color) {
		this->specularColor = color;
	}

	void setAmbientColor(glm::vec3 color) {
		this->ambientColor = color;
	}

	void setShininess(float _shininess) {
		this->shininess = _shininess;
	}

	void setReflectiveness(float _reflectiveness) {
		this->reflectiveness = _reflectiveness;
	}

	void setAlphaBlend(bool blend);

private:
	ShaderProgram* program;
	std::vector <Texture*> textures;
	glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ambientColor = glm::vec3(0.0f, 0.0f, 0.0f);
	float shininess = 30.0f;
	float reflectiveness = 0.0f;

	bool alphaBlend = false;

	class ShaderProgramIsNullptr : public std::exception {};
};

