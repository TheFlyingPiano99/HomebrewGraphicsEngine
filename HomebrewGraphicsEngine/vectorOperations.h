#ifndef VECTOROPERATIONS
#define VECTOROPERATIONS

#include<glm/glm.hpp>

float length(glm::vec3 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

glm::vec3 operator/(glm::vec3 vec, float a) 
{ 
	return glm::vec3(vec.x / a, vec.y / a, vec.z / a);
}

glm::vec3 operator*(glm::vec3 vec, float a)
{
	return glm::vec3(vec.x * a, vec.y * a, vec.z * a);
}

glm::vec3 operator+(glm::vec3 vec1, glm::vec3 vec2)
{
	return glm::vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2);
}

glm::vec3 normalize(glm::vec3 vec)
{
	return vec / length(vec);
}

#endif