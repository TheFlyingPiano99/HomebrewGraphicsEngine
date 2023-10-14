#include "ShaderProgram.h"

#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include "DebugUtils.h"


namespace Hogra {
	
	std::vector<ShaderProgram*> ShaderProgram::instances = std::vector<ShaderProgram*>();

	// Reads a text file and outputs a string with everything in the text file
	std::string ShaderProgram::GetFileContent(const std::filesystem::path& filename) const
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
		DebugUtils::PrintError("ShaderProgram", std::string("Source file \"").append(filename.string()).append("\" can not be opened."));
		throw(errno);
	}

	void ShaderProgram::Init(const std::filesystem::path& vertexFile, const std::filesystem::path& geometryFile, const std::filesystem::path& fragmentFile)
	{
		vertexShaderPath = vertexFile;
		geometryShaderPath = geometryFile;
		fragmentShaderPath = fragmentFile;

		// Read vertexFile and fragmentFile and store the strings
		std::string vertexCode = GetFileContent(vertexFile);
		std::string fragmentCode = GetFileContent(fragmentFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader Object and get its reference
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		// Attach Vertex Shader source to the Vertex Shader Object
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		// Compile the Vertex Shader into machine code
		glCompileShader(vertexShader);
		std::string paths = std::string("{\n\"").append(vertexFile.string()).append("\"\n\"").append(geometryFile.string()).append("\"\n\"").append(fragmentFile.string()).append("\"\n}");
		// Checks if Shader compiled succesfully
		CompileErrors(paths,
			"VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		// Compile the Vertex Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		CompileErrors(paths, "FRAGMENT");

		// Create Shader Program Object and get its reference
		glID = glCreateProgram();
		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(glID, vertexShader);
		glAttachShader(glID, fragmentShader);

		GLuint geometryShader = 0;
		if (!geometryFile.empty()) {	// Geometry shader
			std::string geometryCode = GetFileContent(geometryFile);
			const char* geometrySource = geometryCode.c_str();
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometrySource, nullptr);
			glCompileShader(geometryShader);
			CompileErrors(paths, "GEOMETRY");
			glAttachShader(glID, geometryShader);
		}

		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(glID);
		// Checks if Shaders linked succesfully
		CompileErrors(paths, "PROGRAM");

		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (!geometryFile.empty()) {
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
	void ShaderProgram::CompileErrors(const std::string& paths, const std::string& type) const
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		constexpr int infoMaxLength = 1024 * 16;
		char infoLog[infoMaxLength];
		if (type != "PROGRAM")
		{
			glGetShaderiv(glID, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(glID, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(glID, maxLength, &maxLength, &errorLog[0]);				
				std::string logStr(errorLog.begin(), errorLog.end());
				DebugUtils::PrintError("ShaderProgram",
					std::string("SHADER_COMPILATION_ERROR for: ").append(type).append("\n").append(paths).append("\n").append(logStr).c_str()
				);
			}
		}
		else
		{
			glGetProgramiv(glID, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(glID, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errorLog(maxLength);
				glGetProgramInfoLog(glID, maxLength, &maxLength, &errorLog[0]);
				std::string logStr(errorLog.begin(), errorLog.end());
				DebugUtils::PrintError("ShaderProgram",
					std::string("SHADER_LINKING_ERROR for: ").append(type).append("\n").append(paths).append("\n").append(logStr).c_str()
				);
			}
		}
	}

}