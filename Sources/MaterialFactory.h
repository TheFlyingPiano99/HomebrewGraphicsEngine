#pragma once
#include "Material.h"
#include "TextureCube.h"
#include <string>
#include <map>
#include "MemoryManager.h"

namespace Hogra {
	class MaterialFactory
	{
		friend class Allocator;
	public:
		static MaterialFactory* GetInstance();
		static void DestroyInstance();

		Material* getPBRMaterial(const char* materialName);

		Material* getEmissiveMaterial(const char* materialName, const glm::vec3& color, const float intensity);

		void ForgetPointers();

	private:
		MaterialFactory() = default;
		~MaterialFactory();
		static MaterialFactory* instance;

		std::map<std::string, Material*> loadedPBRMaterials;
	};
}
