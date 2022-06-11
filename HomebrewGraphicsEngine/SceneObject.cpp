#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

void SceneObject::control(float dt) {
	for (auto& component : components) {
		component->control(dt);
	}
}

void SceneObject::update(float dt) {
	for (auto& component : components) {
		component->update(dt);
	}

	translationMatrix = glm::translate(position);
	if (useEulerAngles) {
		rotationMatrix = glm::mat4(1.0f)
			* glm::rotate(eulerAnglesRad.x, glm::vec3(1.0f, 0.0f, 0.0f))
			* glm::rotate(eulerAnglesRad.z, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::rotate(eulerAnglesRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		rotationMatrix = glm::toMat4(orientation);
	}
	scaleMatrix = glm::scale(scale);
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	invModelMatrix = glm::inverse(modelMatrix);
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
