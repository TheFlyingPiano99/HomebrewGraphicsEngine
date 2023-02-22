#include "SceneObject.h"
#include "MemoryManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

namespace Hogra {

	void SceneObject::BeforePhysicsLoopUpdate() {
		for (auto& component : components) {
			component->BeforePhysicsLoopUpdate();
		}
	}

	void SceneObject::EarlyPhysicsUpdate(float dt) {
		for (auto& component : components) {
			component->EarlyPhysicsUpdate(dt);
		}
	}

	void SceneObject::LatePhysicsUpdate(float dt) {
		for (auto& component : components) {
			component->LatePhysicsUpdate(dt);
		}
		for (auto& comp : components) {
			comp->LatePhysicsUpdate(dt);
		}
		if (nullptr != positionConnector) {
			position = positionConnector->GetPosition();
		}
		if (nullptr != orientationConnector) {
			orientation = orientationConnector->GetOrientation();
		}

		translationMatrix = glm::translate(position);
		if (useEulerAngles) {
			rotationMatrix = glm::mat4(1.0f)
				* glm::Rotate(eulerAnglesRad.x, glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::Rotate(eulerAnglesRad.z, glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::Rotate(eulerAnglesRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			rotationMatrix = glm::toMat4(orientation);
		}
		scaleMatrix = glm::scale(scale);
		modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		invModelMatrix = glm::inverse(modelMatrix);
	}

	void SceneObject::AfterPhysicsLoopUpdate() {
		for (auto& component : components) {
			component->AfterPhysicsLoopUpdate();
		}
	}

	void SceneObject::Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera)
	{
		for (auto& comp : components) {
			comp->Draw(outFBO, depthTexture, camera);
		}
		if (nullptr == mesh || !isVisible) {
			return;
		}
		mesh->Bind();
		exportMatrices(*(mesh->getMaterial()->GetShaderProgram()));
		mesh->Draw();
	}

	void SceneObject::DrawShadow(const DirectionalShadowCaster& directionalShadowCaster)
	{
		if (nullptr == mesh) {
			return;
		}
		directionalShadowCaster.GetProgram().SetUniform("sceneObject.modelMatrix", modelMatrix);
		mesh->Draw();
	}

	void SceneObject::exportMatrices(const ShaderProgram& program) const
	{
		program.SetUniform("sceneObject.modelMatrix", modelMatrix);
		program.SetUniform("sceneObject.invModelMatrix", invModelMatrix);
	}
	
	void SceneObject::Serialize() {
		for (auto* component : components) {
			component->Serialize();
		}
	}

}