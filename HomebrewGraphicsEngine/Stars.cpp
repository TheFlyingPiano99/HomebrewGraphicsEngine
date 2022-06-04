#include "Stars.h"
#include "MathUtil.h"

Stars::Stars(int numberOfStars) {

	int resolution = 10000;
	srand(0);
	for (int i = 0; i < numberOfStars / 2; i++) {
		float phi = 2.0 * ((rand() % resolution) / (float)resolution - 0.5) * M_PI;
		float theta = (rand() % resolution) / (float)resolution * M_PI;
		glm::vec3 stardir;
		stardir.x = sinf(phi) * sinf(theta);
		stardir.y = cosf(theta);
		stardir.z = cosf(phi)* sinf(theta);
		stars.push_back(stardir);
	}
}


void Stars::exportStars(ShaderProgram* shader)
{
	glUniform3f(glGetUniformLocation(shader->ID, "starColor"), lightColor.x, lightColor.y, lightColor.z);

	std::string uniform = "stars[";
	for (int i = 0; i < stars.size(); i++) {
		std::string str = uniform;
		glUniform3f(glGetUniformLocation(shader->ID, str.append(std::to_string(i)).append("]").c_str()), stars[i].x, stars[i].y, stars[i].z);
	}
}
