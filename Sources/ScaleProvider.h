/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>

namespace Hogra {
    class ScaleProvider
    {
    public:
        virtual const glm::vec3& GetScale() const = 0;
    };
}

