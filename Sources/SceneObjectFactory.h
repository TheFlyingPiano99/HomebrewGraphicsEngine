#pragma once
#include "MemoryManager.h"
#include "SceneObject.h"
#include "Scene.h"
#include <string>

namespace Hogra {

    class SceneObjectFactory
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:

        static SceneObjectFactory* GetInstance();

        SceneObject* CreateTwoAndHalfDimensionalSpriteObject(const std::filesystem::path& texturePath, OrientationProvider* orientationProvider = nullptr);

        SceneObject* Create2DSpriteObject(const std::filesystem::path& texturePath, const glm::vec3& screenPos, float scale);

    private:
        static SceneObjectFactory* instance;
    };

}

