#include "UniformVariable.h"
#include "glm/glm.hpp"

namespace Hogra {

	
	void UniformVariable<float>::Bind(const ShaderProgram& program) {
		glUniform1f(glGetUniformLocation(program.ID, key), value);
	}

	void UniformVariable<int>::Bind(const ShaderProgram& program) {
		glUniform1i(glGetUniformLocation(program.ID, key), value);
	}

	void UniformVariable<glm::vec2>::Bind(const ShaderProgram& program) {
		glUniform2f(glGetUniformLocation(program.ID, key), value.x, value.y);
	}

	void UniformVariable<glm::vec3>::Bind(const ShaderProgram& program) {
		glUniform3f(glGetUniformLocation(program.ID, key), value.x, value.y, value.z);
	}

	void UniformVariable<glm::vec4>::Bind(const ShaderProgram& program) {
		glUniform4f(glGetUniformLocation(program.ID, key), value.x, value.y, value.z, value.w);
	}

	template<typename T>
	void UniformVariable<T>::Set(const T val)
	{
		value = val;
	}
	
	void UniformVariable<float>::Set(const float val)
	{
		value = val;
	}

	template<typename T>
	const T UniformVariable<T>::Get()
	{
		return value;
	}

	const float UniformVariable<float>::Get()
	{
		return value;
	}
}