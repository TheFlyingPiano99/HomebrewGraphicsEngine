#pragma once
#include "Collider.h"

/*
* A cuboid shaped collider object, that moves, rotates and scales with the SceneObject it is attached to.
* Perfect fit for realistic collision of SceneObejcts with cuboid geometry.
*/
class CuboidCollider :
    public Collider
{
public:

    CuboidCollider(Physics* physics = nullptr) : Collider(physics) {
        type = ColliderType::cuboidColliderType;
    }

    // Inherited via Collider

    bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

    bool testPointInside(const glm::vec3& point) const override;

private:

    bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

    bool collideWithSpherical(const Collider* collider) const override;
    bool collideWithAABB(const Collider* collider) const override;
    bool collideWithCuboid(const Collider* collider) const override;
    bool collideWithComposite(const Collider* collider) const override;
};

