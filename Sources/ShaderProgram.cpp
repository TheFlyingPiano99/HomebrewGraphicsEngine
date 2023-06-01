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
		std::string paths = std::string("{\n\"").append(vertexFile).append("\"\n\"").append(geometryFile).append("\"\n\"").append(fragmentFile).append("\"\n}");
		// Checks if Shader compiled succesfully
		compileErrors(paths,
			"VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		// Compile the Vertex Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		compileErrors(paths, "FRAGMENT");

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
			compileErrors(paths, "GEOMETRY");
			glAttachShader(glID, geometryShader);
		}

		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(glID);
		// Checks if Shaders linked succesfully
		compileErrors(paths, "PROGRAM");

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

	AbstractUniformVariable* ShaderProgram::findUniformVariable(std::string& key) const
	{
		auto iter = std::find_if(uniforms.begin(), uniforms.end(), [key](auto* uni)
			{ return key == uni->getKey(); });
		if (uniforms.end() != iter) {
			return *iter;
		}
		return nullptr;
	}

	// Checks if the different Shaders have compiled properly
	void ShaderProgram::compileErrors(const std::string& paths, const std::string& type) const
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(glID, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(glID, 1024, nullptr, infoLog);
				DebugUtils::PrintError("ShaderProgram", 
					std::string("SHADER_COMPILATION_ERROR for: ").append(type).append("\n").append(paths).append("\n").append(infoLog).c_str()
				);
			}
		}
		else
		{
			glGetProgramiv(glID, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(glID, 1024, nullptr, infoLog);
				DebugUtils::PrintError("ShaderProgram",
					std::string("SHADER_LINKING_ERROR for: ").append(type).append("\n").append(paths).append("\n").append(infoLog).c_str()
				);
			}
		}
	}

}