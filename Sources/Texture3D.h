/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>
#include "MemoryManager.h"
#include"ShaderProgram.h"

namespace Hogra {

    struct Dimensions {
        int width, height, depth, bytesPerVoxel;
    };

    class Texture3D : public Texture
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:
        Texture3D();

        std::vector<char> bytes;

        void Init(const std::string& directory, GLuint slot, GLenum clientDataFormat);

        /*
        * func sholud receive normalized coordinates and outputs normalized intensity
        */
        void Init(glm::ivec3 resolution, std::function<float(float, float, float)> func, GLuint slot, GLenum clientDataFormat);

        void Init(glm::uvec3 dimensions, GLuint unit, GLenum internalFormat, GLenum clientDataFormat, GLenum clientDataType);

        void InitForCompute(glm::uvec3 dimensions, GLuint unit, GLenum internalFormat, GLenum clientDataFormat, GLenum clientDataType);

        void WriteData(void* dataPtr);

        void ReadData(void* dataPtr) const override;

        ~Texture3D() override;

        // Inherited via Texture
        void Bind() const override;

        void Unbind() const override;

        const Dimensions& GetDimensions() const;

        const glm::vec4 ResampleGradientAndDensity(glm::ivec3 position) const;

        float operator()(glm::ivec3 position) const;

        void SetFiltering(GLenum filtering) const override;

    private:
        bool ReadDimensions(const char* path, std::string& name, Dimensions& dimensions);
        Dimensions dimensions;
        int maxValue = 255;
    };
}