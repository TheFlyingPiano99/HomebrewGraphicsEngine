#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "Camera.h"

class Stars
{
	std::vector<glm::vec3> stars;
	glm::vec3 lightColor = glm::vec3(1,1,1);

public:
	Stars(int numberOfStars);
	
	void exportStars(ShaderProgram* shader);
};

