#pragma once
#include <vector>
#include "Texture2D.h"
#include "Camera.h"
#include "Light.h"
#include <span>
#include "ShadowCaster.h"
namespace hograengine {

	class Material
	{
	public:
		Material(ShaderProgram* program) : program(program) {};

		void Bind() const;

		void addTexture(const Texture* texture);
		void clearTextures();

		ShaderProgram* getShaderProgram() const;

		std::span<const Texture* const> getTextures() const;

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
		std::vector <const Texture*> textures;
		glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 ambientColor = glm::vec3(0.0f, 0.0f, 0.0f);
		float shininess = 30.0f;
		float reflectiveness = 0.0f;

		bool alphaBlend = false;

		class ShaderProgramIsNullptr : public std::exception {};
	};

}