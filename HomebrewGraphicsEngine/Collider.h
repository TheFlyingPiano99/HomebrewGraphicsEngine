#pragma once
#include "Ray.h"
#include "Physics.h"

class Collider : public Component
{
public:

	Collider(Physics* physics = nullptr) : physics(physics) {}

	/*
	* Called from scene.
	*/
	void collide(const Collider& collider);
	
	Physics* getPhysics() const;

	void setPhysics(Physics* p) {
		physics = p;
	}

	float getElasticity() const {
		return elasticity;
	}

	void setElasticity(float e) {
		elasticity = e;
	}

	virtual bool testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) = 0;
	
	virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint) = 0;

	enum class ColliderType {
		sphericalColliderType,
		cuboidColliderType,
		undefinedColliderType
	};
	ColliderType type = ColliderType::undefinedColliderType;

	// Inherited via Component
	void control(float dt) override;

	void update(float dt) override;

	const glm::vec3& getPosition() const {
		return position;
	}

	void setPosition(const glm::vec3& p) {
		position = p;
	}

	const glm::vec3& getScale() const {
		return scale;
	}

	void setScale(const glm::vec3& s) {
		scale = s;
	}

	const glm::quat& getOrientation() const {
		return orientation;
	}

	void setOrientation(const glm::quat& o) {
		orientation = o;
	}

protected:

	Physics* physics = nullptr;
	float elasticity = 1.0f;	// From [0..1] interval: 1 ~ fully elastic; 0 ~ inelastic. 
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat orientation = angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 position = glm::vec3(0.0f);
};

