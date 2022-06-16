#pragma once
#include "Camera.h"
#include "VAO.h"
#include "TextureCube.h"
#include "FBO.h"


namespace hograengine {

    class SkyBox
    {
        std::vector<glm::vec3> vertices = {
            // positions          
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),

            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),

            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f)
        };

        VAO vao;
        ShaderProgram* shader = nullptr;
        TextureCube texture;

    public:
        SkyBox(std::vector<std::string>& images, ShaderProgram* shader) : texture(images, 0), shader(shader) {
            vao.Bind();
            // Generates Vertex Buffer Object and links it to vertices
            VBO vbo(vertices);
            // Generates Element Buffer Object and links it to indices
            // Links VBO attributes such as coordinates and colors to VAO
            vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
            // Unbind all to prevent accidentally modifying them
            vao.Unbind();
            vbo.Unbind();
        }

        void draw(FBO& fbo, Camera& camera);

        TextureCube& getTexture() {
            return texture;
        }
    };

}