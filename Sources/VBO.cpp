#include"VBO.h"

namespace Hogra {

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	void VBO::Init(std::vector<Vertex_pos_norm_tang_bitang_uv>& vertices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_pos_norm_tang_bitang_uv), vertices.data(), GL_STATIC_DRAW);
	}

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	void VBO::Init(std::vector<Vertex_pos_norm>& vertices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_pos_norm), vertices.data(), GL_STATIC_DRAW);
	}

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	void VBO::Init(std::vector<Vertex_2d_pos_uv>& vertices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_2d_pos_uv), vertices.data(), GL_STATIC_DRAW);
	}

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	void VBO::Init(std::vector<glm::vec3>& vertices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	}

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	void VBO::Init(std::vector<glm::vec4>& vertices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);
	}

	VBO::~VBO() {
		this->Delete();
	}

	// Binds the VBO
	void VBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, glID);
	}

	// Unbinds the VBO
	void VBO::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Deletes the VBO
	void VBO::Delete()
	{
		glDeleteBuffers(1, &glID);
	}

	void VBO::Load(std::vector<glm::vec3>& vertices)
	{
		glDeleteBuffers(1, &glID);
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	}

	void VBO::Load(std::vector<glm::vec4>& vertices)
	{
		glDeleteBuffers(1, &glID);
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ARRAY_BUFFER, glID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);
	}
}