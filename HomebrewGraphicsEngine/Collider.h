#pragma once
#include "Ray.h"
#include "Physics.h"

class Collider
{
public:

	Collider(Physics* physics = nullptr) : physics(physics) {}

	/*
	* Called from scene.
	*/
	void collide(const Collider& collider);
	
	const Physics* getPhysics() const;

	virtual bool testCollision(const Collider& collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) = 0;
	
	virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint) = 0;
	
private:
	Physics* physics = nullptr;
};

