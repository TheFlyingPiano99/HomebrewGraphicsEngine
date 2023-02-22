#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Animation.h"
#include "FBO.h"
#include "Component.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include <span>
#include "DirectionalShadowCaster.h"
#include "PositionProvider.h"
#include "OrientationProvider.h"
#include "ScaleProvider.h"
#include "MemoryManager.h"
#include "PositionConnector.h"
#include "OrientationConnector.h"
#include "Identifiable.h"


namespace Hogra {

	class SceneObject : public PositionProvider, public OrientationProvider, public ScaleProvider, public Identifiable
	{
		friend class Allocator;
	public:

		void Init(Mesh* _mesh = nullptr) {
			this->mesh = _mesh;
		}

		virtual ~SceneObject() = default;

		void Serialize(std::ostream& stream);

		void BeforePhysicsLoopUpdate();

		void EarlyPhysicsUpdate(float dt);

		void LatePhysicsUpdate(float dt);

		void AfterPhysicsLoopUpdate();

		void Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera);

		void DrawShadow(const DirectionalShadowCaster& directionalShadowCaster);

		void Serialize();

		void UpdateGui() {
			for (auto& comp : components) {
				comp->UpdateGui();
			}
		}

		Mesh* GetMesh() const {
			return mesh;
		}

		void SetMesh(Mesh* _mesh) {
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
		glm::vec3 GetEulerAnglesRad() const {
			return eulerAnglesRad;
		}

		void SetEulerAngles(const glm::vec3 anglesDeg) {
			eulerAnglesRad = glm::vec3(glm::radians(anglesDeg.x), glm::radians(anglesDeg.y), glm::radians(anglesDeg.z));
		}

		void SetUseEulerAngles(const bool b) {
			useEulerAngles = b;
		}

		const glm::quat& GetOrientation() const override {
			return orientation;
		}

		void SetOrientation(const glm::quat _orientation) {
			orientation = _orientation;
		}

		std::span<Component* const> GetComponents() const {
			return components;
		}

		void AddComponent(Component* component) {
			components.push_back(component);
		}

		void RemoveComponent(Component* component) {
			auto iter = std::find(components.begin(), components.end(), component);
			if (components.end() != iter) {
				components.erase(iter);
			}
		}

		glm::mat3 GetScaleMatrix() const {
			return scaleMatrix;
		}

		glm::mat3 GetRotationMatrix() const {
			return rotationMatrix;
		}

		const glm::mat4& GetTranslationMatrix() const {
			return translationMatrix;
		}

		const glm::mat4& GetModelMatrix() const {
			return modelMatrix;
		}

		const glm::mat4& GetInvModelMatrix() const {
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

		void SetPositionConnector(PositionConnector* _connector) {
			positionConnector = _connector;
		}

		void SetOrientationConnector(OrientationConnector* _connector) {
			orientationConnector = _connector;
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

		PositionConnector* positionConnector = nullptr;
		OrientationConnector* orientationConnector = nullptr;

		bool isVisible = true;
		bool isCastingShadow = true;

		/*
		* Exports model matrix and inverse model matrix into shader uniform
		*/
		void exportMatrices(const ShaderProgram& program) const;

		inline void* operator new(std::size_t size) { return ::operator new(size); }
		static std::vector<SceneObject*> heapAllocatedInstances;
	};

}