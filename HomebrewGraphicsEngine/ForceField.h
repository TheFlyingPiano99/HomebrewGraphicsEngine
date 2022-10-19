#pragma once

#include "Component.h"
#include <vector>
#include "Physics.h"
namespace Hogra {

	class ForceField : public Component
	{
	public:
		ForceField() = default;

		// Inherited via Component
		virtual void EarlyPhysicsUpdate(float dt) override = 0;

		virtual void LatePhysicsUpdate(float dt) override = 0;

		std::span<const Physics* const > getListeners() const {
			return listeners;
		}

		void AddListener(Physics* physics) {
			listeners.push_back(physics);
		}

		float GetStrength() const {
			return strength;
		}

		void SetStrength(float str) {
			strength = str;
		}

	protected:
		std::vector<Physics*> listeners;
		float strength = 9.8f;
	};

	class CentralForceField : public ForceField
	{
	public:

		CentralForceField() = default;

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

	};

	class HomogeneForceField : public ForceField
	{
	public:

		HomogeneForceField() = default;

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		const glm::vec3& getDirection() const {
			return direction;
		}

		void setDirection(const glm::vec3& dir) {
			direction = dir;
		}

	private:
		glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	};
}