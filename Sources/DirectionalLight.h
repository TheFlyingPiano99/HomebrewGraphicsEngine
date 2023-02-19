#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "Component.h"
#include "PositionProvider.h"
#include "DirectionalShadowCaster.h"

namespace Hogra {

	class DirectionalLight : public Component {
	public:

		void Init(const glm::vec3& _direction, const glm::vec3& _powerDensity);

		~DirectionalLight() override = default;

		void ExportData(UniformBufferObject& ubo, unsigned int& idx);

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		PositionConnector* getPositionProvider() const {
			return positionConnector;
		}

		void SetPositionProvider(PositionConnector* provider) {
			positionConnector = provider;
		}

		const glm::vec3& GetDirection() const {
			return direction3D;
		}

		const glm::vec4& GetDirection4D() const {
			return direction;
		}

		void SetDirection(const glm::vec3& dir) {
			direction3D = dir;
			direction.x = dir.x;
			direction.y = dir.y;
			direction.z = dir.z;
		}

		const glm::vec3& getPowerDensity() const {
			return powerDensity;
		}

		void SetPowerDensity(const glm::vec3& power) {
			powerDensity = power;
		}

		bool IsActive() const {
			return isActive;
		}

		void SetIsActive(bool b) {
			isActive = b;
		}

		bool IsCastingShadow() const {
			return nullptr != shadowCaster;
		}

		void SetShadowCaster(DirectionalShadowCaster* _shadowCaster) {
			shadowCaster = _shadowCaster;
		}

		DirectionalShadowCaster* GetShadowCaster() const {
			return shadowCaster;
		}

	private:
		glm::vec3 direction3D;
		glm::vec4 direction;
		glm::vec3 powerDensity;
		PositionConnector* positionConnector = nullptr;
		bool isActive = true;
		DirectionalShadowCaster* shadowCaster = nullptr;
	};
}