#pragma once
#include "Collider.h"

namespace Hogra {

    /*
    * Simple and cheap sphere shaped collider object.
    */
    class SphericalCollider :
        public Collider
    {
    public:
        
        void Init(Physics* physics = nullptr) {
            this->physics = physics;
            type = ColliderType::sphericalColliderType;
        }

        ~SphericalCollider() = default;

        // Inherited via Collider
        bool TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        bool TestPointInside(const glm::vec3& point) const override;

        float GetRadius() const {
            return radius;
        }

        void SetRadius(float r) {
            radius = r;
        }

        // Inherited via Collider
        virtual glm::vec3 GetAABBMin() const override;

        virtual glm::vec3 GetAABBMax() const override;

    private:
        float radius = 1.0f;

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