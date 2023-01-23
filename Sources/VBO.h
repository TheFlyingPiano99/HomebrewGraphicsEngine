#pragma once

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>

namespace Hogra {

	// Structure to standardize the vertices used in the meshes
	struct Vertex
	{
		glm::vec3 position;		// layout 0
		glm::vec3 normal;		// layout 1
		glm::vec3 tangent;		// layout 2
		glm::vec3 bitangent;	// layout 3
		glm::vec2 texUV;		// layout 4
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

		void Init(std::vector<Vertex>& vertices);

		void Init(std::vector<glm::vec3>& vertices);

		void Init(std::vector<glm::vec4>& vertices);

		~VBO();

		// Binds the VBO
		void Bind();
		// Unbinds the VBO
		void Unbind();
		// Deletes the VBO
		void Delete();

		void Load(std::vector<glm::vec3>& vertices);

		void Load(std::vector<glm::vec4>& vertices);
	};
}