/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <freetype/freetype.h>  
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include "../ShaderProgram.h"
#include "../GlobalInclude.h"
#include "../ShaderProgramFactory.h"
#include "../AssetFolderPathManager.h"
#include "../Texture2D.h"

namespace Hogra {
#define DEFAULT_FONT_HEIGHT 32

    class Font
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:

        class UnloadedFontException : public std::exception {
        };

        struct Character {
            Texture2D* texture;      // image of the character 
            glm::ivec2   size;       // Size of glyph
            glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
            unsigned int advance;    // Offset to advance to next glyph
        };

        void Init(const std::string& fileName);

        ~Font();

        void RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec3& color);

        Texture2D* RenderTextIntoTexture(const std::wstring& text);

        ShaderProgram* GetShaderProgram() const;


    private:
        bool LoadChar(const std::filesystem::path& path, wchar_t c);

        glm::ivec2 GetTextDimension(const std::wstring& text, int& maxBaseline);

        Character& GetCharacter(wchar_t c);

        std::filesystem::path path;
        ShaderProgram* glyphProgram = nullptr;
        std::map<wchar_t, Character> characters;
        unsigned int vao = 0;
        unsigned int vbo = 0;

        static std::vector<Font*> heapAllocatedInstances;

    };

}

