#pragma once
#include <vector>
#include "Texture2D.h"
#include "Camera.h"
#include "Light.h"
#include <span>
#include "ShadowCaster.h"
#include "MemoryManager.h"

namespace Hogra {

	class Material
	{
		friend class Allocator;
	public:

		void Init(ShaderProgram* program);

		void Bind() const;

		void addTexture(Texture* texture);
		void clearTextures();

		ShaderProgram* GetShaderProgram() const;

		std::span<Texture*> getTextures();

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
		std::vector<Texture*> textures;
		glm::vec3 albedo = glm::vec3(0.95f, 0.95f, 0.95f);
		float roughness = 0.3f;
		float metallic = 0.0f;
		float ao = 0.2f;

		bool alphaBlend = false;
		int srcBlendFunc = GL_SRC_ALPHA;
		int destBlendFunc = GL_ONE_MINUS_SRC_ALPHA;

		class ShaderProgramIsNullptr : public std::exception {};

		inline void* operator new(std::size_t size) { return ::operator new(size); };

	};

}