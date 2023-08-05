#pragma once
#include "Material.h"
#include "TextureCube.h"
#include <string>
#include <map>
#include "MemoryManager.h"

namespace Hogra {
    class MaterialFactory
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:
        static MaterialFactory* GetInstance();
        static void DestroyInstance();

        Material* getPBRMaterial(const std::string& materialName);

        Material* getHomogenousPBRMaterial(glm::vec3 albedo, float roughness, float metallic, float ao);

        Material* getEmissiveMaterial(const char* materialName, const glm::vec3& color, const float intensity);

        Material* getCellShadedMaterial(std::filesystem::path& path);

        Material* getHomogenousCellShadedMaterial(const glm::vec3& color);

        void ForgetPointers();

    private:
        static MaterialFactory* instance;

        std::map<std::string, Material*> loadedPBRMaterials;
    };
}
