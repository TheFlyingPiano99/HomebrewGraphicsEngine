#pragma once

#include<glad/glad.h>
#include<string>
#include <vector>
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "UniformVariable.h"
#include "Identifiable.h"
#include "Texture.h"

namespace Hogra {

	class ShaderProgram : public Identifiable
	{
	public:

		void Init(const std::string& vertexFile, const std::string& geometryFile, const std::string& fragmentFile);

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
			glUniform1i(glGetUniformLocation(glID, name), value->glID);
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

		inline const std::string& GetVertexPath() {
			return vertexShaderPath;
		}

		inline const std::string& GetGeometryPath() {
			return geometryShaderPath;
		}

		inline const std::string& GetFragmentPath() {
			return fragmentShaderPath;
		}

		inline void BindUniformVariable(AbstractUniformVariable* uniform) {
			uniforms.push_back(uniform);
		}

		inline void ClearUniformBindings() {
			uniforms.clear();
		}

	private:
		GLuint glID;

		std::string vertexShaderPath;
		std::string geometryShaderPath;
		std::string fragmentShaderPath;

		// Checks if the different Shaders have compiled properly
		void compileErrors(unsigned int shader, const char* type) const;

		std::string getFileContent(const std::string& filename) const;

		std::vector<AbstractUniformVariable*> uniforms;

		static std::vector<ShaderProgram*> instances;	// Used to reload all shaders

	};

}