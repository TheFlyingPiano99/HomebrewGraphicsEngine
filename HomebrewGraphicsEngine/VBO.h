#pragma once

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>

namespace hograengine {

	// Structure to standardize the vertices used in the meshes
	struct Vertex
	{
		glm::vec4 position;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 texUV;
	};

	struct SimpleVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
	};



	class VBO
	{
	public:
		// Reference ID of the Vertex Buffer Object
		GLuint ID;
		// Constructor that generates a Vertex Buffer Object and links it to vertices
		VBO(std::vector<Vertex>& vertices);
		VBO(std::vector<glm::vec3>& vertices);

		// Binds the VBO
		void Bind();
		// Unbinds the VBO
		void Unbind();
		// Deletes the VBO
		void Delete();

		void Recreate(std::vector<glm::vec3>& vertices);
	};
}