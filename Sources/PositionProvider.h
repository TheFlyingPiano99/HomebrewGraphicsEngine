/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <glm/ext/vector_float3.hpp>

namespace Hogra {
    class PositionProvider
    {
    public:

        virtual const glm::vec3& GetPosition() const = 0;

    };
}

