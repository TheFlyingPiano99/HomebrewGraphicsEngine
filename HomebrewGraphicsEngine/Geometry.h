#pragma once
#include <vector>
#include"VAO.h"
#include"EBO.h"
namespace hograengine {

	class Geometry
	{
	public:
		Geometry(std::vector <Vertex>& vertices, std::vector <GLint>& indices);

		// Bind the VAO to the currently active GPU program
		void Draw();

		void setFaceCulling(bool cull);

	private:
		std::vector <Vertex> vertices;
		std::vector <GLint> indices;
		VAO VAO;

		bool faceCulling = true;
	};

}