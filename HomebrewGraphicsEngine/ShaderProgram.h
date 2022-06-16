#pragma once

#include<glad/glad.h>
#include<string>
namespace hograengine {

	class ShaderProgram
	{
	public:
		// Reference ID of the Shader Program
		GLuint ID;
		// Constructor that build the Shader Program from 2 different shaders
		ShaderProgram(const std::string& vertexFile, const std::string& fragmentFile);

		// Activates the Shader Program
		void Activate() const;
		// Deletes the Shader Program
		void Delete() const;

	private:
		// Checks if the different Shaders have compiled properly
		void compileErrors(unsigned int shader, const char* type) const;

		std::string getFileContent(const std::string& filename) const;
	};
}