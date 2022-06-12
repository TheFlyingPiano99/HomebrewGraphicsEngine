#include "SphericalCollider.h"

bool SphericalCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal)
{
	glm::vec3 a = ray.getPosition() - position;
	float b = glm::dot(a, ray.getDirection());
	float disc = b * b - (glm::dot(a, a) - radius * radius);
	if (0.0f <= disc) {
		float t = -b - sqrtf(disc);
		if (t < 0.0f) {	// Ray start might be inside the sphere
			t = -b + sqrtf(disc);
		}
		if (t < 0.0f) {	// Sphere behind the ray start
			return false;
		}
		wIntersectionPoint = ray.getPosition() + ray.getDirection() * t;
		return true;
	}
	return false;
}

bool SphericalCollider::testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
	bool isCollision = false;
	switch (collider->type)
	{
	case ColliderType::sphericalColliderType:
		isCollision = colliderWithSpherical(collider, wCollisionPoint, wCollisionNormal);
		break;
	case ColliderType::cuboidColliderType:
		break;
	default:
		break;
	}
    return isCollision;
}

bool SphericalCollider::colliderWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
	const auto* sphericalCollider = (const SphericalCollider*)collider;
	glm::vec3 betweenCenters = sphericalCollider->getPosition() - position;
	float l = glm::length(betweenCenters);
	bool isCollision = 0.0f < l && l <= radius + sphericalCollider->getRadius();
	if (isCollision) {
		wCollisionPoint = position + betweenCenters / 2.0f;
		wCollisionNormal = glm::normalize(betweenCenters);
	}
	return isCollision;
}

