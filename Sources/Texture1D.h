#pragma once
#include "Texture.h"
#include<glad/glad.h>
#include<stb/stb_image.h>

#include"ShaderProgram.h"
#include "MemoryManager.h"

namespace Hogra {

    class Texture1D : public Texture
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:
        const char* type;
        unsigned int width = 0;

        void Init(unsigned char* bytes, int width, GLuint unit, GLenum clientDataFormat, GLenum clientDataType);

        void Init(int width, GLuint unit, GLenum internalFormat, GLenum clientDataFormat, GLenum clientDataType);

        ~Texture1D() override {
            this->Delete();
        }

        const unsigned int getWidth() {
            return width;
        }

        // Inherited via Texture
        void Bind() const override;
        void Unbind() const override;
        void SetFiltering(GLenum filtering) const override;

        void WriteData(void* dataPtr) override;

        void ReadData(void* dataPtr) const override;
    };
}