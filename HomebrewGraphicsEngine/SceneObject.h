#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Light.h"
#include "Animation.h"
#include "FBO.h"
#include "Component.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include <span>
#include "ShadowCaster.h"

namespace hograengine {

	class SceneObject
	{
	public:
		SceneObject(Mesh* _mesh = nullptr) : mesh(_mesh) {}

		~SceneObject() = default;

		void control(float dt);

		void update(float dt);

		void draw(const Camera& camera, const std::vector<Light*>& lights, const ShadowCaster& shadowCaster);

		void draw(const ShadowCaster& shadowCaster);

		Mesh* getMesh() const {
			return mesh;
		}

		void setMesh(Mesh* _mesh) {
			mesh = _mesh;
		}

		const glm::vec3& getPosition() const {
			return position;
		}

		void setPosition(const glm::vec3 pos) {
			position = pos;
		}

		glm::vec3 getScale() const {
			return scale;
		}

		void setScale(const glm::vec3 s) {
			scale = s;
		}

		/*
		* Returns angles in radian.
		*/
		glm::vec3 getEulerAngles() const {
			return eulerAnglesRad;
		}

		void setEulerAngles(const glm::vec3 anglesRad) {
			eulerAnglesRad = anglesRad;
		}

		void setUseEulerAngles(const bool b) {
			useEulerAngles = b;
		}

		glm::quat getOrientation() const {
			return orientation;
		}

		void setOrientation(const glm::quat _orientation) {
			orientation = _orientation;
		}

		std::span<Component* const> getComponents() const {
			return components;
		}

		void addComponent(Component* component) {
			components.push_back(component);
		}

		void removeComponent(Component* component) {
			auto iter = std::find(components.begin(), components.end(), component);
			if (components.end() != iter) {
				components.erase(iter);
			}
		}

		glm::mat3 getScaleMatrix() const {
			return scaleMatrix;
		}

		glm::mat3 getRotationMatrix() const {
			return rotationMatrix;
		}

		const glm::mat4& getTranslationMatrix() const {
			return translationMatrix;
		}

	private:
		Mesh* mesh = nullptr;		// Don't delete from this object!
		std::vector<Component*> components;

		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec3 position = glm::vec3(0.0f);
		bool useEulerAngles = false;
		glm::vec3 eulerAnglesRad = glm::vec3(0.0f);
		glm::quat orientation = angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 invModelMatrix = glm::mat4(1.0f);

		/*
		* Exports model matrix and inverse model matrix into shader uniform
		*/
		void exportMatrices(const ShaderProgram& program);
	};

}