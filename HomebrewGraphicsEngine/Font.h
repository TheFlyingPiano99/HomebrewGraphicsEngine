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
#include "ShaderProgramFactory.h"
#include "AssetFolderPathManager.h"

namespace Hogra {
#define DEFAULT_FONT_HEIGHT 24

    class Font
    {
    public:

        class UnloadedFontException : public std::exception {
        };

        struct Character {
            unsigned int textureID;  // ID handle of the glyph texture
            glm::ivec2   size;       // Size of glyph
            glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };

        void Init(const char* fileName) {
            shaderProgram = ShaderProgramFactory::GetInstance()->GetGlyphProgram();
            Load(AssetFolderPathManager::getInstance()->getFontsFolderPath().append(fileName));
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

