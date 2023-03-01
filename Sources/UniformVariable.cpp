#include "UniformVariableImpl.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include<glad/glad.h>


namespace Hogra {
	
	void UniformVariable<float>::Bind(unsigned int id) {
		glUniform1f(glGetUniformLocation(id, key.c_str()), value);
	}

	void UniformVariable<int>::Bind(unsigned int id) {
		glUniform1i(glGetUniformLocation(id, key.c_str()), value);
	}

	void UniformVariable<glm::vec2>::Bind(unsigned int id) {
		glUniform2f(glGetUniformLocation(id, key.c_str()), value.x, value.y);
	}

	void UniformVariable<glm::vec3>::Bind(unsigned int id) {
		glUniform3f(glGetUniformLocation(id, key.c_str()), value.x, value.y, value.z);
	}

	void UniformVariable<glm::vec4>::Bind(unsigned int id) {
		glUniform4f(glGetUniformLocation(id, key.c_str()), value.x, value.y, value.z, value.w);
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