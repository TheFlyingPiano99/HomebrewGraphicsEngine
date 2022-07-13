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

namespace hograengine {
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

        Font(ShaderProgram* program) : shaderProgram(program) {
        }

        ~Font() {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
            for (auto& c : characters) {
                glDeleteTextures(1, &c.second.textureID);
            }
            // Deleting ShaderProgram is not the responsibility of this class
        }

        bool Load(const std::string& path) {
            FT_Library ft;
            if (FT_Init_FreeType(&ft))
            {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                return false;
            }
            FT_Face face;
            if (FT_New_Face(ft, path.c_str(), 0, &face))
            {
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
                return false;
            }
            FT_Set_Pixel_Sizes(face, 0, DEFAULT_FONT_HEIGHT);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

            for (unsigned int c = 31; c <= 50111; c++)
            {
                // load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph '" << c << "'" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x
                };
                characters.insert(std::pair<char, Character>(c, character));
            }

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

        void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
        {
            if (nullptr == shaderProgram) {
                return;
            }
            shaderProgram->Activate();
            glUniform3f(glGetUniformLocation(shaderProgram->ID, "textColor"), color.x, color.y, color.z);
            glm::mat4 projection = glm::ortho(0.0f, (float)GlobalVariables::renderResolutionWidth, 0.0f, (float)GlobalVariables::renderResolutionHeight);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(vao);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);

            // iterate through all characters
            std::string::const_iterator c;
            for (c = text.begin(); c != text.end(); c++)
            {
                auto& ch = characters[*c];

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
                // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
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

        Texture2D* RenderTextInTexture(const std::string& text);
            
        ShaderProgram* getShaderProgram() const {
            return shaderProgram;
        }

        glm::ivec2 getTextDimension(const std::string& text, float& maxBaseline) {
            glm::ivec2 min;
            glm::ivec2 max;
            maxBaseline = 0.0f;
            bool first = true;
            std::string::const_iterator c;
            float x = 0;
            for (c = text.begin(); c != text.end(); c++)
            {
                auto& ch = characters[*c];

                float xpos = x + ch.bearing.x;
                float ypos = ch.bearing.y - ch.size.y;
                float w = ch.size.x;
                float h = ch.size.y;
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
            return max - min;
        }

    private:
        ShaderProgram* shaderProgram = nullptr;
        std::map<char, Character> characters;
        unsigned int vao = 0;
        unsigned int vbo = 0;

    };

}

