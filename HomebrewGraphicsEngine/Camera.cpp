#include"Camera.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

namespace hograengine {

	Camera::Camera(float aspectRatio, glm::vec3 eye, glm::vec3 center)
		: aspectRatio(aspectRatio), eye(eye), center(center)
	{
	}

	bool Camera::update(float dt)
	{
		if (nullptr != animation) {
			animation->perform(this, dt);
		}
		if (nullptr != positionProvider) {
			lookDir = center - eye;
			eye = positionProvider->getPosition() + positionInProvidersSpace;
			center = eye + lookDir;
		}
		if (nullptr != orientationProvider) {
			lookDir = orientationProvider->getOrientation() * lookDirInProvidersSpace;
			center = eye + lookDir;
		}
		// Makes camera look in the right direction from the right position
		view = glm::lookAt(eye + animationOffset, center + animationOffset, prefUp);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), aspectRatio, nearPlane, farPlane);

		viewProjMatrix = projection * view;
		invViewProjMatrix = glm::inverse(viewProjMatrix);
		rayDirMatrix = glm::inverse(viewProjMatrix * glm::translate(eye + animationOffset));
		lookDir = normalize(center - eye);
		right = normalize(cross(lookDir, prefUp));
		up = normalize(cross(right, lookDir));

		bool prevMooved = moved;
		moved = false;
		return prevMooved;
	}

	void Camera::updateOrientation(glm::vec3 newPrefUp)
	{
		right = glm::cross(normalize(center - eye), newPrefUp);
		center = eye + glm::cross(newPrefUp, right);
		prefUp = newPrefUp;
	}

	void Camera::exportMatrices(ShaderProgram& shader, const std::string& uniformaName) const
	{
		std::string copy(uniformaName);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, copy.append(".viewProjMatrix").c_str()), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
		copy = uniformaName;
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, copy.append(".invViewProjMatrix").c_str()), 1, GL_FALSE, glm::value_ptr(invViewProjMatrix));
		copy = uniformaName;
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, copy.append(".rayDirMatrix").c_str()), 1, GL_FALSE, glm::value_ptr(rayDirMatrix));
	}


	void Camera::exportData(ShaderProgram& shader, const std::string& uniformName) const
	{
		std::string copy(uniformName);
		glUniform3f(glGetUniformLocation(shader.ID, copy.append(".position").c_str()), eye.x, eye.y, eye.z);
		copy = uniformName;
		glUniform3f(glGetUniformLocation(shader.ID, copy.append(".center").c_str()), center.x, center.y, center.z);
		copy = uniformName;
		glUniform3f(glGetUniformLocation(shader.ID, copy.append(".right").c_str()), right.x, right.y, right.z);
		copy = uniformName;
		glUniform3f(glGetUniformLocation(shader.ID, copy.append(".up").c_str()), up.x, up.y, up.z);
		copy = uniformName;
		glUniform1f(glGetUniformLocation(shader.ID, copy.append(".FOVrad").c_str()), glm::radians(FOVdeg));
		copy = uniformName;
		glUniform1f(glGetUniformLocation(shader.ID, copy.append(".aspectRatio").c_str()), aspectRatio);
		exportMatrices(shader, uniformName);
	}

	void Camera::exportPostprocessDataAsLightCamera(ShaderProgram& shader)
	{
		glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.eye"), eye.x, eye.y, eye.z);
		glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.center"), center.x, center.y, center.z);
		glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.right"), right.x, right.y, right.z);
		glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.up"), up.x, up.y, up.z);
		glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.FOVrad"), glm::radians(FOVdeg));
		glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.aspectRatio"), aspectRatio);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.invViewProjMatrix"), 1, GL_FALSE, glm::value_ptr(invViewProjMatrix));
	}

	void Camera::moveForward(float dt) {
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * lookDir;
		center += dt * speed * lookDir;
		moved = true;
	}

	void Camera::moveBackward(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -lookDir;
		center += dt * speed * -lookDir;
		moved = true;
	}

	void Camera::moveLeft(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -right;
		center += dt * speed * -right;
		moved = true;
	}

	void Camera::moveRight(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * right;
		center += dt * speed * right;
		moved = true;
	}

	void Camera::moveUp(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * up;
		center += dt * speed * up;
		moved = true;
	}

	void Camera::moveDown(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -up;
		center += dt * speed * -up;
		moved = true;
	}

	void Camera::rotate(float rotX, float rotY)
	{
		if (nullptr != orientationProvider) {
			return;
		}
		glm::quat rotXQuat = angleAxis(rotY, right);
		glm::quat rotYQuat = angleAxis(rotX, prefUp);

		lookDir = normalize(rotYQuat * rotXQuat * lookDir);
		center = eye + lookDir;
	}

	void Camera::approachCenter(float delta)
	{
		if (nullptr != positionProvider) {
			return;
		}
		float l = length(center - eye);
		if (l > delta) {
			eye += delta * normalize(center - eye) * approachCenterSpeed;
		}
		moved = true;
	}
}