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
#include "ShaderProgram.h"
#include "Texture2D.h"

namespace Hogra {

    class TextureCube : public Texture
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:
        friend class FBO;

        const char* type;

        void Init(std::vector<std::filesystem::path>& images, GLuint unit, GLuint clientDataType);

        void Init(unsigned int resolution, GLuint unit, GLenum internalFormat, GLenum clientDataFormat, GLenum clientDataType, bool useLinearFiltering = false);

        void InitFromEquirectangular(const Texture2D& equirectangularMap, unsigned int unit, GLenum internalFormat, GLenum clientDataFormat, GLenum clientDataType);

        void InitFromCubeMap(
            const TextureCube& cubemap,
            ShaderProgram& conversionShader,
            unsigned int resolution,
            unsigned int unit,
            GLuint clientDataFormat,
            GLuint clientDataType,
            unsigned int maxMipLevels = 1
        );

        ~TextureCube() override {
            this->Delete();
        }

        // Binds a texture
        void Bind();
        // Unbinds a texture
        void Unbind();
        // Deletes a texture
        void Delete();

        const glm::ivec2& GetDimensions() const {
            return dimensions;
        }

        // Inherited via Texture
        virtual void Bind() const override;

        virtual void Unbind() const override;

        void SetFiltering(GLenum filtering) const override;

        Texture2D GetFace(unsigned int face);

        void WriteData(void* dataPtr) override;

        void ReadData(void* dataPtr) const override;

    private:
        glm::ivec2 dimensions;
        glm::vec4 nullVector;

    };
}