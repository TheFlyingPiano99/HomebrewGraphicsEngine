#pragma once

#include<glad/glad.h>
#include<string>
#include <vector>
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Hogra {

	class ShaderProgram
	{
	public:

		// Reference ID of the Shader Program
		GLuint ID;

		void Init(const std::string& vertexFile, const std::string& geometryFile, const std::string& fragmentFile);

		ShaderProgram();

		~ShaderProgram();

		// Activates the Shader Program
		void Activate() const;
		// Deletes the Shader Program
		void Delete() const;

		void SetUniform(const char* name, const glm::mat4& value) const {
			glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
		}

		void SetUniform(const char* name, const float value) const {
			glUniform1f(glGetUniformLocation(ID, name), value);
		}

		void SetUniform(const char* name, const int value) const {
			glUniform1i(glGetUniformLocation(ID, name), value);
		}

		void SetUniform(const char* name, const glm::vec2& value) const {
			glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
		}

		void SetUniform(const char* name, const glm::vec3& value) const {
			glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
		}

		void SetUniform(const char* name, const glm::vec4& value) const {
			glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
		}

		void SetUniform(const char* name, const glm::ivec2& value) const {
			glUniform2i(glGetUniformLocation(ID, name), value.x, value.y);
		}

		void SetUniform(const char* name, const glm::ivec3& value) const {
			glUniform3i(glGetUniformLocation(ID, name), value.x, value.y, value.z);
		}

		void SetUniform(const char* name, const glm::ivec4& value) const {
			glUniform4i(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
		}

		void SetUniform(const char* name, const glm::mat<2, 2, float, glm::packed_highp>& value) const {
			glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
		}

		void SetUniform(const char* name, const glm::mat<3, 3, float, glm::packed_highp>& value) const {
			glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
		}

		static void ReloadAll() {
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

		const std::string& GetVertexPath() {
			return vertexShaderPath;
		}

		const std::string& GetGeometryPath() {
			return geometryShaderPath;
		}

		const std::string& GetFragmentPath() {
			return fragmentShaderPath;
		}

	private:

		std::string vertexShaderPath;
		std::string geometryShaderPath;
		std::string fragmentShaderPath;

		// Checks if the different Shaders have compiled properly
		void compileErrors(unsigned int shader, const char* type) const;

		std::string getFileContent(const std::string& filename) const;

		static std::vector<ShaderProgram*> instances;	// Used to reload all shaders
	};

}