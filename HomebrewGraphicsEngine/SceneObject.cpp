#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

void SceneObject::update() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(position) 
		* glm::rotate(eulerAngles.x, glm::vec3(1,0,0)) 
		* glm::rotate(eulerAngles.y, glm::vec3(0, 1, 0)) 
		* glm::rotate(eulerAngles.z, glm::vec3(0, 0, 1))
		* glm::scale(scale);
	invModelMatrix = glm::inverse(modelMatrix);
}

void SceneObject::animate(float dt)
{
	if (nullptr != animation) {
		animation->perform(this, dt);
	}
	update();
}

void SceneObject::draw(const Camera& camera, const std::vector<Light*>& lights)
{
	if (nullptr == mesh) {
		return;
	}
	mesh->Bind(camera, lights);
	exportMatrices(*(mesh->getMaterial()->getShaderProgram()));
	mesh->Draw();
}

void SceneObject::exportMatrices(const ShaderProgram& program)
{
	glUniformMatrix4fv(glGetUniformLocation(program.ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program.ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
}
