#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "Component.h"
#include "PositionProvider.h"
#include "OmniDirectionalShadowCaster.h"

namespace Hogra {

	class PointLight : public Component, public PositionProvider {
	public:

		void Init(const glm::vec3& _position, const glm::vec3& _powerDensity);

		~PointLight() override = default;

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

		const glm::vec3& GetPosition() const override {
			return position3D;
		}

		const glm::vec4& GetPosition4D() const {
			return position;
		}

		void SetPosition(const glm::vec3& pos) {
			position3D = pos;
			position.x = pos.x;
			position.y = pos.y;
			position.z = pos.z;
		}

		const glm::vec3& getPowerDensity() const {
			return powerDensity;
		}

		void SetPowerDensity(const glm::vec3& power) {
			powerDensity = power;
		}

		float getEffectiveRadius() const;

		const glm::mat4& GetVolumeModelMatrix() const {
			return volumeModelMatrix;
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

		void SetShadowCaster(OmniDirectionalShadowCaster* _shadowCaster) {
			shadowCaster = _shadowCaster;
		}

		OmniDirectionalShadowCaster* GetShadowCaster() const {
			return shadowCaster;
		}

	private:
		glm::vec3 position3D;
		glm::vec4 position;
		glm::vec3 powerDensity;
		glm::mat4 volumeModelMatrix;
		PositionConnector* positionConnector = nullptr;
		float effectiveRadius;
		bool isActive = true;
		OmniDirectionalShadowCaster* shadowCaster = nullptr;
	};
}