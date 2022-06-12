#include "SphericalCollider.h"

bool SphericalCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint)
{
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

