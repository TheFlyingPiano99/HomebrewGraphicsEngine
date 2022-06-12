#include "CuboidCollider.h"

bool CuboidCollider::testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
    return false;
}

bool CuboidCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal)
{
    return false;
}

void CuboidCollider::control(float dt)
{
}

void CuboidCollider::update(float dt)
{
}
