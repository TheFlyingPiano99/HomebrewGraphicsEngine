#include "Font.h"
#include "FBO.h"
#include "GeometryFactory.h"
#include "glm/gtx/transform.hpp"
#include "DebugUtils.h"

namespace Hogra {

    std::vector<Font*> Font::heapAllocatedInstances = std::vector<Font*>();

    Font::~Font() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        // Deleting ShaderProgram is not the responsibility of this class
    }

    bool Font::LoadChar(const std::string& path, wchar_t c) {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            DebugUtils::PrintError("Freetype font loading", "Could not init FreeType Library");
            return false;
        }
        FT_Face face;
        if (FT_New_Face(ft, path.c_str(), 0, &face))
        {
            DebugUtils::PrintError("Freetype font loading", "Failed to load font");
            return false;
        }
        FT_Set_Pixel_Sizes(face, 0, DEFAULT_FONT_HEIGHT);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            auto str = std::string("Failed to load Glyph ");
            str.append((const char*)c);
            DebugUtils::PrintError("Freetype font loading", str.c_str());
        }
        Texture2D* charTexture = Allocator::New<Texture2D>();
        charTexture->Init((const char*)face->glyph->bitmap.buffer, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), (GLuint)0, (GLenum)GL_RED, (GLenum)GL_RED, (GLenum)GL_UNSIGNED_BYTE);
        charTexture->SetFiltering(GL_NEAREST);

        // now store character for later use
        Character character = {
            charTexture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        characters.emplace(std::pair<wchar_t, Character>(c, character));

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        return true;
    }

    void Font::RenderText(const std::wstring& text, float x, float y, float scale, const glm::vec3& color)
    {
        if (nullptr == glyphProgram) {
            return;
        }
        glyphProgram->Activate();
        glyphProgram->SetUniform("textColor", color);
        glm::mat4 projection = glm::ortho(0.0f, (float)GlobalVariables::windowWidth, (float)GlobalVariables::windowHeight, 0.0f);
        glyphProgram->SetUniform("transform", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        // iterate through all characters
        std::wstring::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            auto& ch = GetCharacter(*c);

            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            ch.texture->Bind();
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture2D* Font::RenderTextIntoTexture(const std::wstring& text) {
        if (nullptr == glyphProgram) {
            throw UnloadedFontException();
        }
        int baseline;
        glm::ivec2 dim = GetTextDimension(text, baseline);

        Texture2D* textureToReturn = Allocator::New<Texture2D>();
        textureToReturn->Init(GL_RED, dim, 0, GL_RED, GL_UNSIGNED_BYTE);

        FBO fbo;
        fbo.Init();
        fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *textureToReturn);
        fbo.Bind();
        glDisable(GL_BLEND);
        auto quad = GeometryFactory::GetInstance()->GetSimpleQuad();
        glyphProgram->Activate();
        glm::mat4 projection = glm::ortho(0.0f, (float)dim.x, 0.0f, (float)dim.y);

        // iterate through all characters
        std::wstring::const_iterator c;
        int x = 0;
        int y = baseline;
        for (c = text.begin(); c != text.end(); c++)
        {
            const auto& ch = GetCharacter(*c);
            float xpos = x + ch.bearing.x + ch.size.x * 0.5f;
            float ypos = y - (ch.size.y - ch.bearing.y) + ch.size.y * 0.5f;
            auto transform = projection * glm::translate(glm::vec3(xpos, ypos, 0.0f)) * glm::scale(glm::vec3(ch.size.x, -ch.size.y, 2.0f) * 0.5f);
            glyphProgram->SetUniform("transform", transform);
            ch.texture->Bind();
            quad->Draw();
            x += (ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        fbo.Unbind();
        fbo.Delete();
        return textureToReturn;
    }

    Font::Character& Font::GetCharacter(wchar_t c)
    {
        if (characters.contains(c)) {
            return characters.find(c)->second;
        }
        LoadChar(path, c);
        return characters.find(c)->second;
    }
    
    ShaderProgram* Font::GetShaderProgram() const {
        return glyphProgram;
    }
    
    glm::ivec2 Font::GetTextDimension(const std::wstring& text, int& maxBaseline) {
        glm::ivec2 min;
        glm::ivec2 max;
        maxBaseline = 0.0f;
        bool first = true;
        std::wstring::const_iterator c;
        float x = 0.0f;
        for (c = text.begin(); c != text.end(); c++)
        {
            const auto& ch = GetCharacter(*c);

            int xpos = x + ch.bearing.x;
            int ypos = ch.bearing.y - ch.size.y;
            int w = ch.size.x;
            int h = ch.size.y;
            if (maxBaseline < ch.size.y - ch.bearing.y) {
                maxBaseline = ch.size.y - ch.bearing.y;
            }
            if (first || xpos < min.x) {
                min.x = xpos;
            }
            if (first || ypos < min.y) {
                min.y = ypos;
            }
            if (first || xpos + w > max.x) {
                max.x = xpos + w;
            }
            if (first || ypos + h > max.y) {
                max.y = ypos + h;
            }
            if (first) {
                first = false;
            }
            x += (ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        return max - min + glm::ivec2(1);
    }
}