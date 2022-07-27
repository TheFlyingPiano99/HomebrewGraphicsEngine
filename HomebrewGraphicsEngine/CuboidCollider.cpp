#include "CuboidCollider.h"

namespace Hogra {

    bool CuboidCollider::TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::TestPointInside(const glm::vec3& point) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithSpherical(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithAABB(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithCuboid(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool CuboidCollider::CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return collider->CollideWithCuboid(this, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CuboidCollider::CollideWithComposite(const Collider* collider) const
    {
        return collider->CollideWithCuboid(this);
    }

    glm::vec3 CuboidCollider::GetAABBMin() const
    {
        return glm::vec3();
    }

    glm::vec3 CuboidCollider::GetAABBMax() const
    {
        return glm::vec3();
    }

}
