#pragma once
#include<glad/glad.h>
#include<vector>

namespace hograengine {

	class EBO
	{
	public:
		// ID reference of Elements Buffer Object
		GLuint ID;
		// Constructor that generates a Elements Buffer Object and links it to indices
		EBO(std::vector<GLint>& indices);

		void Recreate(std::vector<GLint>& indices);

		// Binds the EBO
		void Bind();
		// Unbinds the EBO
		void Unbind();
		// Deletes the EBO
		void Delete();
	};
}