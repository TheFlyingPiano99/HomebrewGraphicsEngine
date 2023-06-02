#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "UniformVariable.h"
#include "Identifiable.h"
#include "Texture.h"

namespace Hogra {

	class ShaderProgram : public Identifiable
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		void Init(const std::filesystem::path& vertexFile, const std::filesystem::path& geometryFile, const std::filesystem::path& fragmentFile);

		ShaderProgram();

		~ShaderProgram();

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
				if (!instance->GetVertexPath().empty() && !instance->GetFragmentPath().empty()) {
					instance->Init(
						instance->GetVertexPath(),
						instance->GetGeometryPath(),
						instance->GetFragmentPath()
					);
				}
			}
		}

		inline const std::filesystem::path& GetVertexPath() {
			return vertexShaderPath;
		}

		inline const std::filesystem::path& GetGeometryPath() {
			return geometryShaderPath;
		}

		inline const std::filesystem::path& GetFragmentPath() {
			return fragmentShaderPath;
		}

		inline void BindUniformVariable(AbstractUniformVariable* uniform) {
			uniforms.push_back(uniform);
		}

		inline void ClearUniformBindings() {
			uniforms.clear();
		}

		AbstractUniformVariable* findUniformVariable(std::string& key) const;

	private:
		GLuint glID;

		std::filesystem::path vertexShaderPath;
		std::filesystem::path geometryShaderPath;
		std::filesystem::path fragmentShaderPath;

		// Checks if the different Shaders have compiled properly
		void CompileErrors(const std::string& paths, const std::string& type) const;

		std::string GetFileContent(const std::filesystem::path& filename) const;

		std::vector<AbstractUniformVariable*> uniforms;

		static std::vector<ShaderProgram*> instances;	// Used to reload all shaders

	};

}