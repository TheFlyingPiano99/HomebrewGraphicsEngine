#include "ComputeProgram.h"
#include "DebugUtils.h"
#include<fstream>

namespace Hogra {

	std::vector<ComputeProgram*> ComputeProgram::instances = std::vector<ComputeProgram*>();

	
	void ComputeProgram::Init(const std::filesystem::path& path)
	{
		sourcePath = path;
		if (!path.has_filename()) {
			DebugUtils::PrintError("ComputeProgram", "Failed to initialise compute shader from source code from not fully specified path!");
			return;
		}

		std::string sourceCode = getFileContent(sourcePath);
		const char* source_c_str = sourceCode.c_str();
		GLuint glcomputeID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(glcomputeID, 1, &source_c_str, nullptr);
		glCompileShader(glcomputeID);
		compileErrors(glcomputeID, "COMPUTE");

		glID = glCreateProgram();
		glAttachShader(glID, glcomputeID);
		glLinkProgram(glID);
		compileErrors(glID, "PROGRAM");

		glDeleteShader(glcomputeID);
	}


	ComputeProgram::ComputeProgram()
	{
		instances.push_back(this);
	}
	

	ComputeProgram::~ComputeProgram()
	{
		Delete();
		auto iter = std::find(instances.begin(), instances.end(), this);
		instances.erase(iter);
	}


	void ComputeProgram::Activate() const
	{
		glUseProgram(glID);
		for (auto& uniform : uniforms) {
			uniform->Bind(glID);
		}
	}


	void ComputeProgram::Delete() const
	{
		glDeleteProgram(glID);
	}


	AbstractUniformVariable* ComputeProgram::findUniformVariable(std::string& key) const
	{
		auto iter = std::find_if(uniforms.begin(), uniforms.end(), [key](auto* uni)
			{ return key == uni->getKey(); });
		if (uniforms.end() != iter) {
			return *iter;
		}
		return nullptr;
	}

	void ComputeProgram::Dispatch()
	{
		glDispatchComputeGroupSizeARB(
			numberOfWorkGroups.x, numberOfWorkGroups.y, numberOfWorkGroups.z,
			workGroupSize.x, workGroupSize.y, workGroupSize.z
		);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}


	void ComputeProgram::compileErrors(unsigned int shader, const char* type) const {
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

	void ComputeProgram::SetNumberOfWorkGroups(glm::uvec3 groups) {
		numberOfWorkGroups = groups;

		int max_work_grp_cnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_work_grp_cnt[2]);
		if (groups.x > max_work_grp_cnt[0]) {
			DebugUtils::PrintError("ComputeProgram", "Trying to set work group count in X dimension to higher value than max allowed!");
			numberOfWorkGroups.x = max_work_grp_cnt[0];
		}
		if (groups.y > max_work_grp_cnt[1]) {
			DebugUtils::PrintError("ComputeProgram", "Trying to set work group count in Y dimension to higher value than max allowed!");
			numberOfWorkGroups.y = max_work_grp_cnt[1];
		}
		if (groups.z > max_work_grp_cnt[2]) {
			DebugUtils::PrintError("ComputeProgram", "Trying to set work group count in Z dimension to higher value than max allowed!");
			numberOfWorkGroups.z = max_work_grp_cnt[2];
		}
	}

	void ComputeProgram::SetWorkGroupSize(glm::uvec3 size)
	{
		workGroupSize = size;
	}


	std::string ComputeProgram::getFileContent(const std::filesystem::path& path) const
	{
		if (std::ifstream in(path, std::ios::binary); in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		DebugUtils::PrintError("ComputeProgram", 
			std::string("Source file can not be opened: \"").append(path.string()).append("\"").c_str()
		);
		throw(errno);
	}

}
