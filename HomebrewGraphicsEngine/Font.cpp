#include "Font.h"
#include "FBO.h"
namespace Hogra {
    Texture2D* Font::RenderTextInTexture(const std::string& text) {
        if (nullptr == shaderProgram) {
            throw UnloadedFontException();
        }
        int baseline;
        glm::ivec2 dim = GetTextDimension(text, baseline);
        Texture2D* texture = new Texture2D();
        texture->Init(GL_RGBA, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE);
        FBO fbo;
        fbo.Init();
        fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *texture);
        fbo.Bind();

        shaderProgram->Activate();
        glm::mat4 projection = glm::ortho(0.0f, (float)dim.x, 0.0f, (float)dim.y);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        // iterate through all characters
        std::string::const_iterator c;
        int x = 0;
        int y = baseline;
        for (c = text.begin(); c != text.end(); c++)
        {
            auto& ch = characters[*c];

            int xpos = x + ch.bearing.x;
            int ypos = y - (ch.size.y - ch.bearing.y);

            int w = ch.size.x;
            int h = ch.size.y;
            // update VBO for each character
            float vertices[6][4] = {
                { (float)xpos,     ypos + h,   0.0f, 0.0f },
                { (float)xpos,     ypos,       0.0f, 1.0f },
                { (float)xpos + w, ypos,       1.0f, 1.0f },

                { (float)xpos,     ypos + h,   0.0f, 0.0f },
                { (float)(xpos + w), ypos,       1.0f, 1.0f },
                { (float)(xpos + w), ypos + h,   1.0f, 0.0f }
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
            x += (ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        fbo.Unbind();
        fbo.Delete();
        return texture;
    }
}