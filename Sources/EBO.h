#pragma once
#include<glad/glad.h>
#include<vector>

namespace Hogra {

	class EBO
	{
	public:
		// ID reference of Elements Buffer Object
		GLuint glID;

		void Init(const std::vector<GLuint>& indices);

		void Load(const std::vector<GLuint>& indices);

		// Bind's the EBO
		void Bind();
		// Unbind's the EBO
		void Unbind();
		// Delete's the EBO
		void Delete();
	};
}