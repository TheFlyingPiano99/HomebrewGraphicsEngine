#pragma once

#include <filesystem>
#include<glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "MemoryManager.h"
#include "ShaderStorageBuffer.h"
#include "UniformVariable.h"
#include "Texture.h"



namespace Hogra {

    class ComputeProgram
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:

        void Init(const std::filesystem::path& sourcePath);

        ComputeProgram();

        ~ComputeProgram();

        // Activates the Shader Program
        void Activate() const;

        // Deletes the Shader Program
        void Delete() const;

        inline void SetUniform(const char* name, const glm::mat4& value) const {
            glUniformMatrix4fv(glGetUniformLocation(glID, name), 1, GL_FALSE, glm::value_ptr(value));
        }

        inline void SetUniform(const char* name, const float value) const {
            glUniform1f(glGetUniformLocation(glID, name), value);
        }

        inline void SetUniform(const char* name, const int value) const {
            glUniform1i(glGetUniformLocation(glID, name), value);
        }

        inline void SetUniform(const char* name, const glm::vec2& value) const {
            glUniform2f(glGetUniformLocation(glID, name), value.x, value.y);
        }

        inline void SetUniform(const char* name, const glm::vec3& value) const {
            glUniform3f(glGetUniformLocation(glID, name), value.x, value.y, value.z);
        }

        inline void SetUniform(const char* name, const glm::vec4& value) const {
            glUniform4f(glGetUniformLocation(glID, name), value.x, value.y, value.z, value.w);
        }

        inline void SetUniform(const char* name, const glm::ivec2& value) const {
            glUniform2i(glGetUniformLocation(glID, name), value.x, value.y);
        }

        inline void SetUniform(const char* name, const glm::ivec3& value) const {
            glUniform3i(glGetUniformLocation(glID, name), value.x, value.y, value.z);
        }

        inline void SetUniform(const char* name, const glm::ivec4& value) const {
            glUniform4i(glGetUniformLocation(glID, name), value.x, value.y, value.z, value.w);
        }

        inline void SetUniform(const char* name, const glm::mat<2, 2, float, glm::packed_highp>& value) const {
            glUniformMatrix2fv(glGetUniformLocation(glID, name), 1, GL_FALSE, glm::value_ptr(value));
        }

        inline void SetUniform(const char* name, const glm::mat<3, 3, float, glm::packed_highp>& value) const {
            glUniformMatrix3fv(glGetUniformLocation(glID, name), 1, GL_FALSE, glm::value_ptr(value));
        }

        inline void SetUniform(const char* name, const Texture* value) const {
            glUniform1i(glGetUniformLocation(glID, name), value->unit);
        }

        inline static void ReloadAll() {
            for (auto instance : instances) {
                instance->Delete();
                if (!instance->GetSourcePath().empty()) {
                    instance->Init(instance->GetSourcePath());
                }
            }
        }

        inline std::string GetSourcePath() {
            return sourcePath.filename().string();
        }

        inline void BindUniformVariable(AbstractUniformVariable* uniform) {
            uniforms.push_back(uniform);
        }

        inline void ClearUniformBindings() {
            uniforms.clear();
        }

        AbstractUniformVariable* findUniformVariable(std::string& key) const;

        void SetNumberOfWorkGroups(glm::uvec3 groups);

        void Dispatch() const;

        static glm::uvec3 GetMaxNumberOfWorkGroup();

        static glm::uvec3 GetMaxWorkGroupSize();

        static unsigned int GetMaxInvocationsPerWorkGroup();

        inline unsigned int GetGlID() const {
            return glID;
        }

    private:
        GLuint glID;

        std::filesystem::path sourcePath;

        glm::uvec3 numberOfWorkGroups = glm::vec3(1, 1, 1);


        // Checks if the different Shaders have compiled properly
        void CompileErrors(unsigned int shader, const char* type) const;

        std::string GetFileContent(const std::filesystem::path& filename) const;

        std::vector<AbstractUniformVariable*> uniforms;

        static std::vector<ComputeProgram*> instances;	// Used to reload all shaders

    };

}

