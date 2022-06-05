#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_HfirstC
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

#include "ShaderProgram.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 eye;
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 prefUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 viewProjMatrix = glm::mat4(1.0f);
	glm::mat4 invViewProjMatrix = glm::mat4(1.0f);
	glm::mat4 rayDirMatrix = glm::mat4(1.0f);
	float FOVdeg = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 2000.0f;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	bool moved = false;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 100.0f;
	float approachCenterSpeed = 10.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 eye, glm::vec3 center);

	// Updates the camera matrix to the Vertex Shader
	bool update();

	void updateOrientation(glm::vec3 prefUp);
	// Exports the camera matrix to a shader
	void exportMatrices(ShaderProgram& shader, const std::string& uniformaName = "camera") const;

	void exportData(ShaderProgram& program, const std::string& uniformName = "camera") const;
	void exportPostprocessDataAsLightCamera(ShaderProgram& program);

	// Obsolete
	// Handles camera inputs
	void Inputs(GLFWwindow* window);

	void moveForward(float dt);
	void moveBackward(float dt);
	void moveLeft(float dt);
	void moveRight(float dt);
	void moveUp(float dt);
	void moveDown(float dt);

	void rotate(float mouseX, float mouseY);
	void rotateAroundBullseye(float mouseX, float mouseY, glm::vec3 bullseye);
	void approachCenter(float delta);
};
#endif