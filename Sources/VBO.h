#pragma once

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>

namespace Hogra {

	// Structure to standardize the vertices used in the meshes
	struct Vertex_pos_norm_tang_bitang_uv
	{
		glm::vec3 position;		// layout 0
		glm::vec3 normal;		// layout 1
		glm::vec3 tangent;		// layout 2
		glm::vec3 bitangent;	// layout 3
		glm::vec2 uv;		// layout 4
	};

	struct Vertex_pos_norm
	{
		glm::vec3 position;
		glm::vec3 normal;
	};

	struct Vertex_2d_pos_uv
	{
		glm::vec2 position;
		glm::vec2 uv;
	};


	class VBO
	{
	public:
		// Reference ID of the Vertex Buffer Object
		GLuint glID;

		void Init(std::vector<Vertex_pos_norm_tang_bitang_uv>& vertices);

		void Init(std::vector<Vertex_pos_norm>& vertices);

		void Init(std::vector<Vertex_2d_pos_uv>& vertices);

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