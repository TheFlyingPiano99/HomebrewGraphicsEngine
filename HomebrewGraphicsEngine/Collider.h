#pragma once
#include "Ray.h"

class Collider
{
public:

	virtual bool testRayIntersection(Ray& ray, glm::vec3& wIntersectionPoint) = 0;
	virtual bool testCollision(Collider& collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) = 0;
};

