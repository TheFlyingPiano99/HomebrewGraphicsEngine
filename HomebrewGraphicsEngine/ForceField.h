#pragma once

#include "Component.h"
#include <vector>
#include "Physics.h"

class ForceField : public Component
{
public:
	ForceField() = default;

	// Inherited via Component
	virtual void control(float dt) override = 0;

	virtual void update(float dt) override = 0;

	std::span<const Physics* const > getListeners() const {
		return listeners;
	}

	void addListener(Physics* physics) {
		listeners.push_back(physics);
	}

	float getStrength() const {
		return strength;
	}

	void setStrength(float str) {
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
	void control(float dt) override;

	void update(float dt) override;

};

class HomogeneForceField : public ForceField
{
public:
	HomogeneForceField() = default;

	// Inherited via Component
	void control(float dt) override;

	void update(float dt) override;

	const glm::vec3& getDirection() const {
		return direction;
	}

	void setDirection(const glm::vec3& dir) {
		direction = dir;
	}

private:
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
};
