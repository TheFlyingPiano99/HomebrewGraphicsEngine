#pragma once
#include "Collider.h"

namespace Hogra {
    /*
    * A cuboid shaped collider object, that moves, rotates and scales with the SceneObject it is attached to.
    * Perfect fit for realistic collision of SceneObejcts with cuboid geometry.
    */
    class CuboidCollider :
        public Collider
    {
    public:
        void Init(Physics* physics = nullptr) {
            this->physics = physics;
            type = ColliderType::cuboidColliderType;
        }

        // Inherited via Collider

        bool TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        bool TestPointInside(const glm::vec3& point) const override;

        // Inherited via Collider
        virtual glm::vec3 GetAABBMin() const override;
        virtual glm::vec3 GetAABBMax() const override;

    private:

        bool CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool CollideWithSpherical(const Collider* collider) const override;
        bool CollideWithAABB(const Collider* collider) const override;
        bool CollideWithCuboid(const Collider* collider) const override;
        bool CollideWithComposite(const Collider* collider) const override;

    };
}

