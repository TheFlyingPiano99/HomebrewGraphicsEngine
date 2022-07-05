#include "CuboidCollider.h"

namespace hograengine {

    bool CuboidCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::testPointInside(const glm::vec3& point) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithSpherical(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithAABB(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithCuboid(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return collider->collideWithCuboid(this, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CuboidCollider::collideWithComposite(const Collider* collider) const
    {
        return collider->collideWithCuboid(this);
    }

    glm::vec3 CuboidCollider::getAABBMin() const
    {
        return glm::vec3();
    }

    glm::vec3 CuboidCollider::getAABBMax() const
    {
        return glm::vec3();
    }

}
