#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ShaderProgram.h"
#include "PositionConnector.h"
#include "OrientationConnector.h"
#include "Animation.h"
#include "UniformBuffer.h"

namespace Hogra {

	class Camera : public PositionProvider, public OrientationProvider
	{
	public:

		void Init(float aspectRatio, glm::vec3 eye, glm::vec3 lookAt);
			
		// Updates the camera matrix to the Vertex Shader
		bool Update();

		// Updates the camera matrix to the Vertex Shader
		void LatePhysicsUpdate(float dt);

		void UpdatePreferedUp(glm::vec3 prefUp);

		/*
		* Load data on GPU.
		*/
		void ExportData();

		void MoveForward(float dt);
		void MoveBackward(float dt);
		void MoveLeft(float dt);
		void MoveRight(float dt);
		void MoveUp(float dt);
		void MoveDown(float dt);

		void Rotate(const glm::vec2& deltaAngle);
		
		void RotateAroundPoint(const glm::vec2& deltaAngle, const glm::vec3& rotationCenter = glm::vec3(0.0f));

		void ApproachCenter(float delta, const glm::vec3& center = glm::vec3(0.0f));

		void setAspectRatio(float ratio) {
			aspectRatio = ratio;
		}

		void setMoved(bool val) {
			moved = val;
		}

		const glm::mat4& GetProjectionMatrix() const {
			return projection;
		}

		const glm::mat4& GetViewProjMatrix() const {
			return viewProjMatrix;
		}

		const glm::mat4& GetInvViewProjMatrix() const {
			return invViewProjMatrix;
		}

		const glm::mat4& GetRayDirMatrix() const {
			return rayDirMatrix;
		}

		const glm::vec3& GetLookDir() const {
			return lookDir;
		}

		const glm::vec3& GetPosition() const override {
			return eye;
		}

		void SetPositionConnetor(PositionConnector* provider) {
			positionConnector = provider;
		}

		void SetOrientationConnector(OrientationConnector* provider) {
			orientationConnector = provider;
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

		const glm::quat& GetOrientation() const override {
			return orientation;
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

		bool PopIsMoved() {
			auto temp = moved;
			moved = false;
			return temp;
		}

	private:
		UniformBufferObject ubo;
		// Stores the main vectors of the camera
		glm::vec3 eye;										// The position of the virtual viewers eye
		glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);		// A point in world-space that the camera is looking at
		glm::vec3 prefUp = glm::vec3(0.0f, 1.0f, 0.0f);		// A unit vector pointing in the prefered "up" direction
		glm::vec3 lookDir = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 viewProjMatrix = glm::mat4(1.0f);
		glm::mat4 invViewProjMatrix = glm::mat4(1.0f);
		glm::mat4 rayDirMatrix = glm::mat4(1.0f);
		glm::quat orientation;
		float FOVdeg = 45.0f;
		float nearPlane = 0.1f;
		float farPlane = 2000.0f;

		// Prevents the camera from jumping around when first clicking left click
		bool firstClick = true;

		bool moved = false;
		PositionConnector* positionConnector = nullptr;
		OrientationConnector* orientationConnector = nullptr;

		Animation* animation = nullptr;
		glm::vec3 animationOffset = glm::vec3(0.0f);

		// Adjust the speed of the camera and it's sensitivity when looking around
		float speed = 0.1f;
		float sensitivity = 100.0f;
		float aspectRatio;
	};
}