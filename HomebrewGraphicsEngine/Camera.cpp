#include"Camera.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "GlobalInclude.h"

namespace Hogra {


	void Camera::Init(float aspectRatio, glm::vec3 eye, glm::vec3 center) {
		this->aspectRatio = aspectRatio;
		this->eye = eye;
		this->center = center;
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

	bool Camera::Update(float dt)
	{
		if (nullptr != animation) {
			animation->perform(this, dt);
		}
		if (nullptr != positionProvider) {
			lookDir = center - eye;
			eye = positionProvider->GetPosition() + positionInProvidersSpace;
			center = eye + lookDir;
		}
		if (nullptr != orientationProvider) {
			lookDir = orientationProvider->GetOrientation() * lookDirInProvidersSpace;
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

	void Camera::Rotate(float rotX, float rotY)
	{
		if (nullptr != orientationProvider) {
			return;
		}
		glm::quat rotXQuat = angleAxis(rotY, right);
		glm::quat rotYQuat = angleAxis(rotX, prefUp);

		auto possibleLookDir = normalize(rotYQuat * rotXQuat * lookDir);
		if (abs(glm::dot(possibleLookDir, prefUp)) < 0.9f) {
			lookDir = possibleLookDir;
			center = eye + lookDir;
		}
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