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
#include "PositionProvider.h"
#include "OrientationProvider.h"
#include "ScaleProvider.h"

namespace Hogra {

	class SceneObject : public PositionProvider, public OrientationProvider, public ScaleProvider
	{
	public:

		static SceneObject* Instantiate();

		void Init(Mesh* _mesh = nullptr) {
			this->mesh = _mesh;
		}

		static void Deallocate(SceneObject* instance)
		{
			auto iter = std::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), instance);
			if (iter != heapAllocatedInstances.end()) {
				heapAllocatedInstances.erase(iter);
				delete instance;
			}
		}

		static void DeallocateAll() {
			for (auto& instance : heapAllocatedInstances) {
				delete instance;
			}
			heapAllocatedInstances.clear();
		}

		~SceneObject() = default;

		void FrameBeginningControl();

		void Control(float dt);

		void Update(float dt, const Camera& camera);

		void FrameEndingControl();

		void Draw();

		void drawShadow(const ShadowCaster& shadowCaster);

		Mesh* GetMesh() const {
			return mesh;
		}

		void setMesh(Mesh* _mesh) {
			mesh = _mesh;
		}

		const glm::vec3& GetPosition() const override {
			return position;
		}

		void SetPosition(const glm::vec3 pos) {
			position = pos;
		}

		const glm::vec3& GetScale() const override {
			return scale;
		}

		void SetScale(const glm::vec3 s) {
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

		const glm::quat& GetOrientation() const override {
			return orientation;
		}

		void SetOrientation(const glm::quat _orientation) {
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

		const glm::mat4& getModelMatrix() const {
			return modelMatrix;
		}

		const glm::mat4& getInvModelMatrix() const {
			return invModelMatrix;
		}

		bool IsVisible() const {
			return isVisible;
		}

		void SetIsVisible(bool v) {
			isVisible = v;
		}

		void SetIsCastingShadow(bool v) {
			isCastingShadow = v;
		}

		bool IsCastingShadow() const {
			return isCastingShadow;
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

		bool isVisible = true;
		bool isCastingShadow = true;

		/*
		* Exports model matrix and inverse model matrix into shader uniform
		*/
		void exportMatrices(const ShaderProgram& program);

		inline void* operator new(std::size_t size) { return ::operator new(size); }
		static std::vector<SceneObject*> heapAllocatedInstances;
	};

}