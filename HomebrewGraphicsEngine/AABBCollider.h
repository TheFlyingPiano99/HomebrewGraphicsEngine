#pragma once
#include "Collider.h"
namespace hograengine {

    /*
    * Cheap axis alligned bounding box collider.
    */
    class AABBCollider :
        public Collider
    {
    public:
        explicit AABBCollider(Physics* physics = nullptr) : Collider(physics) {
            type = ColliderType::AABBColliderType;
        }

        // Inherited via Collider

        virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        virtual bool testPointInside(const glm::vec3& point) const override;

        void control(float dt) override {
        }

        void update(float dt) override {
            if (nullptr != physics) {
                position = physics->getOwnerPosition();
                scale = physics->getOwnerScale();
                orientation = physics->getOwnerOrientation();
            }
            min = minInOrigo + position;
            max = maxInOrigo + position;
        }

        /*
        * Used for spatial tree
        */
        const glm::vec3& getMin() const {
            return min;
        }

        /*
        * Used for spatial tree
        */
        void setMin(const glm::vec3& _min) {
            min = _min;
        }

        /*
        * Used for spatial tree
        */
        const glm::vec3& getMax() const {
            return max;
        }

        /*
        * Used for spatial tree
        */
        void setMax(const glm::vec3& _max) {
            max = _max;
        }

        const glm::vec3& getMinInOrigo() const {
            return minInOrigo;
        }

        void setMinInOrigo(const glm::vec3& _min) {
            minInOrigo = _min;
        }

        const glm::vec3& getMaxInOrigo() const {
            return maxInOrigo;
        }

        void setMaxInOrigo(const glm::vec3& _max) {
            maxInOrigo = _max;
        }

        // Inherited via Collider
        glm::vec3 getAABBMin() override;
        glm::vec3 getAABBMax() override;

    private:
        glm::vec3 min = glm::vec3();
        glm::vec3 max = glm::vec3();

        glm::vec3 minInOrigo = glm::vec3();
        glm::vec3 maxInOrigo = glm::vec3();

        bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
        bool collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool collideWithSpherical(const Collider* collider) const override;
        bool collideWithAABB(const Collider* collider) const override;
        bool collideWithCuboid(const Collider* collider) const override;
        bool collideWithComposite(const Collider* collider) const override;
    };
}
