#pragma once
#include <vector>
#include "Texture2D.h"
#include "Camera.h"
#include "Light.h"
#include <span>
#include "ShadowCaster.h"
namespace Hogra {

	class Material
	{
	public:
		Material(ShaderProgram* program) : program(program) {};

		void Bind() const;

		void addTexture(const Texture* texture);
		void clearTextures();

		ShaderProgram* getShaderProgram() const;

		std::span<const Texture* const> getTextures() const;

		glm::vec3 getAlbedo() const {
			return albedo;
		}

		void setAlbedo(const glm::vec3& c) {
			albedo = c;
		}

		float getRoughness() const {
			return roughness;
		}

		void setRoughness(float a) {
			roughness = a;
		}

		float getMetallic() const {
			return metallic;
		}

		void setMetallic(float m) {
			metallic = m;
		}

		float getAO() const {
			return ao;
		}

		void setAO(float _ao) {
			ao = _ao;
		}


		void setAlphaBlend(bool blend);

		void setBlendFunc(int src, int dest);

	private:
		ShaderProgram* program;
		std::vector <const Texture*> textures;
		glm::vec3 albedo = glm::vec3(0.95f, 0.95f, 0.95f);
		float roughness = 0.3f;
		float metallic = 0.0f;
		float ao = 0.2f;

		bool alphaBlend = false;
		int srcBlendFunc = GL_ONE;
		int destBlendFunc = GL_ONE;

		class ShaderProgramIsNullptr : public std::exception {};
	};

}