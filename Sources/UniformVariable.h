/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <string>

namespace Hogra {

    class AbstractUniformVariable {
    public:
        AbstractUniformVariable() = default;
        virtual ~AbstractUniformVariable() = default;
        virtual void Bind(unsigned int id) = 0;

        std::string_view getKey() const { return key; }

    protected:
        std::string key;
    };

}
