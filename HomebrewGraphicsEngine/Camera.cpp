#include"Camera.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "GlobalInclude.h"

namespace Hogra {


	void Camera::Init(float aspectRatio, glm::vec3 eye, glm::vec3 lookAt) {
		this->aspectRatio = aspectRatio;
		this->eye = eye;
		this->lookAt = lookAt;
		std::vector<int> uniformDataSizes;
		//eye:
		uniformDataSizes.push_back(sizeof(glm::vec3));
		//viewProjMatrix:
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		//rayDirMatrix:
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		uniformDataSizes.push_back(sizeof(glm::vec4));
		ubo.Init(uniformDataSizes, CAMERA_UBO_BINDING);
	}

	bool Camera::Update()
	{
		// Makes camera look in the right direction from the right position
		view = glm::lookAt(eye + animationOffset, lookAt + animationOffset, prefUp);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), aspectRatio, nearPlane, farPlane);

		viewProjMatrix = projection * view;
		invViewProjMatrix = glm::inverse(viewProjMatrix);
		rayDirMatrix = glm::inverse(viewProjMatrix * glm::translate(eye + animationOffset));
		bool prevMooved = moved;
		moved = false;
		return prevMooved;
	}

	void Camera::LatePhysicsUpdate(float dt)
	{
		if (nullptr != animation) {
			animation->perform(this, dt);
		}
		if (nullptr != positionProvider) {
			eye = positionProvider->GetPosition() + positionInProvidersSpace;
			lookAt = eye + lookDir;
		}
		if (nullptr != orientationProvider) {
			lookDir = glm::normalize(orientationProvider->GetOrientation() * lookDirInProvidersSpace);
			right = glm::cross(lookDir, prefUp);
			up = glm::cross(right, lookDir);
			lookAt = eye + lookDir;
		}
	}

	void Camera::UpdatePreferedUp(glm::vec3 newPrefUp)
	{
		prefUp = newPrefUp;
		right = glm::cross(lookDir, prefUp);
		up = glm::cross(right, lookDir);
	}

	void Camera::ExportData()
	{
		ubo.Bind();
		//eye:
		ubo.UploadSubData((void*)glm::value_ptr(eye), 0);
		//viewProjMatrix:
		ubo.UploadSubData((void*)glm::value_ptr(viewProjMatrix[0]), 1);
		ubo.UploadSubData((void*)glm::value_ptr(viewProjMatrix[1]), 2);
		ubo.UploadSubData((void*)glm::value_ptr(viewProjMatrix[2]), 3);
		ubo.UploadSubData((void*)glm::value_ptr(viewProjMatrix[3]), 4);
		//rayDirMatrix:
		ubo.UploadSubData((void*)glm::value_ptr(rayDirMatrix[0]), 5);
		ubo.UploadSubData((void*)glm::value_ptr(rayDirMatrix[1]), 6);
		ubo.UploadSubData((void*)glm::value_ptr(rayDirMatrix[2]), 7);
		ubo.UploadSubData((void*)glm::value_ptr(rayDirMatrix[3]), 8);
		ubo.Unbind();
	}

	void Camera::MoveForward(float dt) {
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * lookDir;
		lookAt += dt * speed * lookDir;
		moved = true;
	}

	void Camera::MoveBackward(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -lookDir;
		lookAt += dt * speed * -lookDir;
		moved = true;
	}

	void Camera::MoveLeft(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -right;
		lookAt += dt * speed * -right;
		moved = true;
	}

	void Camera::MoveRight(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * right;
		lookAt += dt * speed * right;
		moved = true;
	}

	void Camera::MoveUp(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * up;
		lookAt += dt * speed * up;
		moved = true;
	}

	void Camera::MoveDown(float dt)
	{
		if (nullptr != positionProvider) {
			return;
		}
		eye += dt * speed * -up;
		lookAt += dt * speed * -up;
		moved = true;
	}

	void Camera::Rotate(const glm::vec2& deltaAngle)
	{
		if (nullptr != orientationProvider) {
			return;
		}
		glm::quat rotXQuat = angleAxis(deltaAngle.y, right);
		glm::quat rotYQuat = angleAxis(deltaAngle.x, prefUp);

		auto possibleLookDir = normalize(rotYQuat * rotXQuat * lookDir);
		if (abs(glm::dot(possibleLookDir, prefUp)) < 0.9f) {
			lookDir = possibleLookDir;
		}
		else {
			lookDir = normalize(rotYQuat * lookDir);
		}
		lookAt = eye + lookDir;
		right = glm::cross(lookDir, prefUp);
		up = glm::cross(right, lookDir);
		moved = true;
	}

	void Camera::RotateAroundPoint(const glm::vec2& deltaAngle, const glm::vec3& rotationCenter)
	{
		auto toCenter = rotationCenter - eye;
		glm::quat rotXQuat = angleAxis(deltaAngle.y, right);
		glm::quat rotYQuat = angleAxis(deltaAngle.x, prefUp);
		glm::vec3 rotatedToCenter;
		auto possibleLookDir = normalize(rotYQuat * rotXQuat * lookDir);
		if (abs(glm::dot(possibleLookDir, prefUp)) < 0.9f) {
			rotatedToCenter = rotYQuat * rotXQuat * toCenter;
		}
		else {			// If the rotation around X axis causes gimbal lock than execute only the rotation around Y axis
			rotatedToCenter = rotYQuat * toCenter;
		}
		lookDir = glm::normalize(rotatedToCenter);
		eye = rotationCenter - rotatedToCenter;
		lookAt = eye + lookDir;
		right = glm::cross(lookDir, prefUp);
		up = glm::cross(right, lookDir);
		moved = true;
	}

	void Camera::ApproachCenter(float delta, const glm::vec3& center)
	{
		if (nullptr != positionProvider) {
			return;
		}
		float l = length(center - eye);
		if (l - 0.01f > delta) {	// If not passing the lookAt position
			eye += delta * normalize(center - eye);
			lookAt = eye + lookDir;
			moved = true;
		}
	}

}