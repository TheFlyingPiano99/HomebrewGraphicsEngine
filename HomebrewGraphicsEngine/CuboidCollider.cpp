#include "CuboidCollider.h"

bool CuboidCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal)
{
    //TODO
    return false;
}

bool CuboidCollider::testPointInside(const glm::vec3& point)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithSpherical(const Collider* collider)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithAABB(const Collider* collider)
{
    //TODO
    return false;
}

bool CuboidCollider::collideWithCuboid(const Collider* collider)
{
    //TODO
    return false;
}
