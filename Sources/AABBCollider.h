#pragma once
#include "Collider.h"
namespace Hogra {

    /*
    * Cheap axis alligned bounding box collider.
    */
    class AABBCollider :
        public Collider
    {
    public:

        void Init(Physics* physics = nullptr) {
            this->physics = physics;
            type = ColliderType::AABBColliderType;
        }

        // Inherited via Collider

        virtual bool TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        virtual bool TestPointInside(const glm::vec3& point) const override;

        void EarlyPhysicsUpdate(float dt) override {
        }

        void LatePhysicsUpdate(float dt) override {
            if (nullptr != positionProvider) {
                position = positionProvider->GetPosition();
            }
            min = minRelToPosition + position;
            max = maxRelToPosition + position;
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
        void SetMin(const glm::vec3& _min) {
            min = _min;
        }

        /*
        * Used for spatial tree
        */
        const glm::vec3& GetMax() const {
            return max;
        }

        /*
        * Used for spatial tree
        */
        void SetMax(const glm::vec3& _max) {
            max = _max;
        }

        const glm::vec3& getMinRelToPosition() const {
            return minRelToPosition;
        }

        void setMinRelToPosition(const glm::vec3& _min) {
            minRelToPosition = _min;
        }

        const glm::vec3& getMaxRelToPosition() const {
            return maxRelToPosition;
        }

        void setMaxRelToPosition(const glm::vec3& _max) {
            maxRelToPosition = _max;
        }

        // Inherited via Collider
        glm::vec3 GetAABBMin() const override;
        glm::vec3 GetAABBMax() const override;

    private:
        glm::vec3 min = glm::vec3();
        glm::vec3 max = glm::vec3();

        glm::vec3 minRelToPosition = glm::vec3();
        glm::vec3 maxRelToPosition = glm::vec3();

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
