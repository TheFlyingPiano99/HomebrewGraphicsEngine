#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "Component.h"
#include "PositionProvider.h"

namespace Hogra {

	class Light : public Component, public PositionProvider {
	public:

		void Init(const glm::vec4& _position, const glm::vec3& _powerDensity);

		~Light() = default;

		void ExportData(UniformBufferObject& ubo, unsigned int& idx);

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void Update() override;

		PositionProvider* getPositionProvider() const {
			return positionProvider;
		}

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
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

		const glm::mat4& getVolumeModelMatrix() const {
			return volumeModelMatrix;
		}

		bool IsActive() const {
			return isActive;
		}

		void SetIsActive(bool b) {
			isActive = b;
		}

		void SetCastShadow(bool b) {
			castShadow = b;
		}

		bool IsCastShadow() const {
			return castShadow;
		}

	private:
		glm::vec3 position3D;
		glm::vec4 position;
		glm::vec3 powerDensity;
		glm::mat4 volumeModelMatrix;
		PositionProvider* positionProvider;
		float effectiveRadius;
		bool isActive = true;
		bool castShadow = true;
	};
}