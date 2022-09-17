#pragma once
#include <freetype/freetype.h>  
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include "ShaderProgram.h"
#include "GlobalInclude.h"
#include <glm/gtc/type_ptr.hpp>
#include "Texture2D.h"

namespace Hogra {
#define DEFAULT_FONT_HEIGHT 24

    class Font
    {
    public:

        static Font* Instantiate();

        static void Deallocate(Font* instance)
        {
            auto iter = std::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), instance);
            if (iter != heapAllocatedInstances.end()) {
                heapAllocatedInstances.erase(iter);
                delete instance;
            }
        }

        static void DeallocateAll() {
            for (auto& instance : heapAllocatedInstances) {
                delete instance;
            }
            heapAllocatedInstances.clear();
        }

        class UnloadedFontException : public std::exception {
        };

        struct Character {
            unsigned int textureID;  // ID handle of the glyph texture
            glm::ivec2   size;       // Size of glyph
            glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };

        void Init(ShaderProgram* program) {
            shaderProgram = program;
        }

        ~Font();

        bool Load(const std::string& path);

        void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

        Texture2D* RenderTextInTexture(const std::string& text);
            
        ShaderProgram* GetShaderProgram() const;

        glm::ivec2 GetTextDimension(const std::string& text, int& maxBaseline);

    private:
        ShaderProgram* shaderProgram = nullptr;
        std::map<char, Character> characters;
        unsigned int vao = 0;
        unsigned int vbo = 0;

        static std::vector<Font*> heapAllocatedInstances;

    };

}

