#pragma once
#include<glad/glad.h>
#include<vector>

namespace Hogra {

	class EBO
	{
	public:
		// ID reference of Elements Buffer Object
		GLuint ID;

		void Init(std::vector<GLint>& indices);

		void Load(std::vector<GLint>& indices);

		// Bind's the EBO
		void Bind();
		// Unbind's the EBO
		void Unbind();
		// Delete's the EBO
		void Delete();
	};
}