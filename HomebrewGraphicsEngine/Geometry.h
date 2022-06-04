#pragma once
#include <vector>
#include"VAO.h"
#include"EBO.h"

class Geometry
{
public:
	Geometry(std::vector <Vertex>& vertices, std::vector <GLint>& indices);

	// Bind the VAO to the currently active GPU program
	void Draw();

private:
	std::vector <Vertex> vertices;
	std::vector <GLint> indices;
	VAO VAO;
};

