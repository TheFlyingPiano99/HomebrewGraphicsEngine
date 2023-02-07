#include "ShaderProgram.h"

#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include "DebugUtils.h"


namespace Hogra {
	
	std::vector<ShaderProgram*> ShaderProgram::instances = std::vector<ShaderProgram*>();

	// Reads a text file and outputs a string with everything in the text file
	std::string ShaderProgram::getFileContent(const std::string& filename) const
	{
		if (std::ifstream in(filename, std::ios::binary); in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		DebugUtils::PrintError("ShaderProgram", "Source file can not be opened.");
		throw(errno);
	}

	void ShaderProgram::Init(const std::string& vertexFile, const std::string& geometryFile, const std::string& fragmentFile)
	{
		vertexShaderPath = vertexFile;
		geometryShaderPath = geometryFile;
		fragmentShaderPath = fragmentFile;

		// Read vertexFile and fragmentFile and store the strings
		std::string vertexCode = getFileContent(vertexFile);
		std::string fragmentCode = getFileContent(fragmentFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader Object and get its reference
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		// Attach Vertex Shader source to the Vertex Shader Object
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		// Compile the Vertex Shader into machine code
		glCompileShader(vertexShader);
		// Checks if Shader compiled succesfully
		compileErrors(vertexShader, "VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		// Compile the Vertex Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		compileErrors(fragmentShader, "FRAGMENT");

		// Create Shader Program Object and get its reference
		glID = glCreateProgram();
		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(glID, vertexShader);
		glAttachShader(glID, fragmentShader);

		GLuint geometryShader = 0;
		if (0 < geometryFile.length()) {	// Geometry shader
			std::string geometryCode = getFileContent(geometryFile);
			const char* geometrySource = geometryCode.c_str();
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometrySource, nullptr);
			glCompileShader(geometryShader);
			compileErrors(geometryShader, "GEOMETRY");
			glAttachShader(glID, geometryShader);
		}

		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(glID);
		// Checks if Shaders linked succesfully
		compileErrors(glID, "PROGRAM");

		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (0 < geometryFile.length()) {
			glDeleteShader(geometryShader);
		}
	}

	ShaderProgram::ShaderProgram() {
		instances.push_back(this);
	}

	ShaderProgram::~ShaderProgram() {
		Delete();
		auto iter = std::find(instances.begin(), instances.end(), this);
		instances.erase(iter);
	}

	// Activates the Shader Program
	void ShaderProgram::Activate() const
	{
		glUseProgram(glID);
		for (auto& uniform : uniforms) {
			uniform->Bind(glID);
		}
	}

	// Deletes the Shader Program
	void ShaderProgram::Delete() const
	{
		glDeleteProgram(glID);
	}

	// Checks if the different Shaders have compiled properly
	void ShaderProgram::compileErrors(unsigned int shader, const char* type) const
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
	}

}