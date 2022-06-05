#include"Camera.h"
#include <iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

Camera::Camera(int width, int height, glm::vec3 eye, glm::vec3 center)
	: width(width), height(height), eye(eye), center(center)
{
}

bool Camera::update()
{
	// Makes camera look in the right direction from the right position
	view = glm::lookAt(eye, center, prefUp);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	viewProjMatrix = projection * view;
	invViewProjMatrix = glm::inverse(viewProjMatrix);
	rayDirMatrix = glm::inverse(viewProjMatrix * glm::translate(eye));
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
	glUniform1f(glGetUniformLocation(shader.ID, copy.append(".aspectRatio").c_str()), width / (float)height);
	exportMatrices(shader, uniformName);
}

void Camera::exportPostprocessDataAsLightCamera(ShaderProgram& shader)
{
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.eye"), eye.x, eye.y, eye.z);
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.center"), center.x, center.y, center.z);
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.right"), right.x, right.y, right.z);
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.up"), up.x, up.y, up.z);
	glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.FOVrad"), glm::radians(FOVdeg));
	glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.aspectRatio"), width / (float)height);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.invViewProjMatrix"), 1, GL_FALSE, glm::value_ptr(invViewProjMatrix));
}


void Camera::Inputs(GLFWwindow* window)
{
	glm::vec3 lookDir = normalize(center - eye);
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		eye += speed * lookDir;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		eye += speed * -glm::normalize(glm::cross(lookDir, prefUp));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		eye += speed * -lookDir;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		eye += speed * glm::normalize(glm::cross(lookDir, prefUp));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		eye += speed * prefUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		eye += speed * -prefUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}



	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(lookDir, glm::radians(-rotX), glm::normalize(glm::cross(lookDir, prefUp)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, prefUp) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			lookDir = newOrientation;
		}

		// Rotates the Orientation left and right
		lookDir = glm::rotate(lookDir, glm::radians(-rotY), prefUp);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}

void Camera::moveForward(float dt) {
	eye += dt * speed * lookDir;
	center += dt * speed * lookDir;
	moved = true;
}

void Camera::moveBackward(float dt)
{
	eye += dt * speed * -lookDir;
	center += dt * speed * -lookDir;
	moved = true;
}

void Camera::moveLeft(float dt)
{
	eye += dt * speed * -right;
	center += dt * speed * -right;
	moved = true;
}

void Camera::moveRight(float dt)
{
	eye += dt * speed * right;
	center += dt * speed * right;
	moved = true;
}

void Camera::moveUp(float dt)
{
	eye += dt * speed * up;
	center += dt * speed * up;
	moved = true;
}

void Camera::moveDown(float dt)
{
	eye += dt * speed * -up;
	center += dt * speed * -up;
	moved = true;
}

void Camera::rotate(float mouseX, float mouseY)
{
	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	// and then "transforms" them into degrees 
	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
	glm::vec3 lookDir = normalize(center - eye);

	// Calculates upcoming vertical change in the Orientation
	glm::vec3 newOrientation = glm::rotate(lookDir, glm::radians(-rotX), glm::normalize(glm::cross(lookDir, prefUp)));

	// Decides whether or not the next vertical Orientation is legal or not
	if (abs(glm::angle(newOrientation, prefUp) - glm::radians(90.0f)) <= glm::radians(85.0f))
	{
		lookDir = newOrientation;
	}

	// Rotates the Orientation left and right
	lookDir = glm::rotate(lookDir, glm::radians(-rotY), prefUp);
	center = eye + lookDir;
	moved = true;
}

void Camera::rotateAroundBullseye(float mouseX, float mouseY, glm::vec3 bullseye)
{
	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
// and then "transforms" them into degrees 
	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

	glm::vec3 toBullseye = bullseye - eye;
	glm::vec3 lookDir = normalize(center - eye);

	// Calculates upcoming vertical change in the Orientation
	glm::vec3 newTOBullseye = glm::rotate(toBullseye, glm::radians(-rotX), glm::normalize(glm::cross(lookDir, prefUp)));

	// Decides whether or not the next vertical Orientation is legal or not
	if (abs(glm::angle(normalize(newTOBullseye), prefUp) - glm::radians(90.0f)) <= glm::radians(85.0f))
	{
		toBullseye = newTOBullseye;
	}
	// Rotates the Orientation left and right
	toBullseye = glm::rotate(toBullseye, glm::radians(-rotY), prefUp);
	eye = bullseye - toBullseye;
	moved = true;
}

void Camera::approachCenter(float delta)
{
	float l = length(center - eye);
	if (l > delta) {
		eye += delta * normalize(center - eye) * approachCenterSpeed;
	}
	moved = true;
}
