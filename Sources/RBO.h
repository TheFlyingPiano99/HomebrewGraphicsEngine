/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <glad/glad.h>
#include <vector>
namespace Hogra {

    class RBO
    {
    public:
        friend class FBO;
        GLuint glID;

        void Init(GLenum internalformat, GLsizei width, GLsizei height);

        void Bind();

        void Delete();
    };

}