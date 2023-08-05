/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <glm/ext/quaternion_float.hpp>

namespace Hogra {
    class OrientationProvider
    {
    public:
        virtual const glm::quat& GetOrientation() const = 0;
    };
}
