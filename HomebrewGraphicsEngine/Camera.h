#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ShaderProgram.h"
#include "PositionProvider.h"
#include "OrientationProvider.h"
#include "Animation.h"

namespace hograengine {

	class Camera
	{
	public:
		// Camera constructor to set up initial values
		Camera(float aspectRatio, glm::vec3 eye, glm::vec3 center);

		// Updates the camera matrix to the Vertex Shader
		bool update(float dt);

		void updateOrientation(glm::vec3 prefUp);
		// Exports the camera matrix to a shader
		void exportMatrices(ShaderProgram& shader, const std::string& uniformaName = "camera") const;

		void exportData(ShaderProgram& program, const std::string& uniformName = "camera") const;
		void exportPostprocessDataAsLightCamera(ShaderProgram& program);

		void moveForward(float dt);
		void moveBackward(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void moveUp(float dt);
		void moveDown(float dt);

		void rotate(float mouseX, float mouseY);
		void approachCenter(float delta);

		void setAspectRatio(float ratio) {
			aspectRatio = ratio;
		}

		void setMoved(bool val) {
			moved = val;
		}

		const glm::mat4& getProjectionMatrix() const {
			return projection;
		}

		const glm::mat4& getViewProjMatrix() const {
			return viewProjMatrix;
		}

		const glm::mat4& getInvViewProjMatrix() const {
			return invViewProjMatrix;
		}

		const glm::mat4& getRayDirMatrix() const {
			return rayDirMatrix;
		}

		const glm::vec3& getLookDir() const {
			return lookDir;
		}

		const glm::vec3& getEyePos() const {
			return eye;
		}

		const PositionProvider* getPositionProvider() const {
			return positionProvider;
		}

		void setPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		const OrientationProvider* getOrientationProvider() const {
			return orientationProvider;
		}

		void setOrientationProvider(OrientationProvider* provider) {
			orientationProvider = provider;
		}

		const glm::vec3& getRight() const {
			return right;
		}

		const glm::vec3& getUp() const {
			return up;
		}

		const glm::vec3& getPreferedUp() const {
			return prefUp;
		}

		glm::quat getOrientation() {
			return quat_cast(glm::inverse(view));
		}

		void setPositionInProvidersSpace(glm::vec3 pos) {
			positionInProvidersSpace = pos;
		}

		Animation* getAnimation() const {
			return animation;
		}

		void setAnimation(Animation* _animation) {
			animation = _animation;
		}

		void setAnimationOffset(const glm::vec3& offset) {
			animationOffset = offset;
		}

	private:
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
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionInProvidersSpace = glm::vec3(0.0f);
		OrientationProvider* orientationProvider = nullptr;
		glm::vec3 lookDirInProvidersSpace = glm::vec3(0.0f, 0.0f, 1.0f);

		Animation* animation = nullptr;
		glm::vec3 animationOffset = glm::vec3(0.0f);

		// Adjust the speed of the camera and it's sensitivity when looking around
		float speed = 0.1f;
		float sensitivity = 100.0f;
		float approachCenterSpeed = 10.0f;
		float aspectRatio;
	};
}