#pragma once
#include "Collider.h"
#include <functional>

namespace Hogra {

    class CompositeCollider :
        public Collider
    {
    public:

        void Init(Physics* physics = nullptr) {
            this->physics = physics;
            type = ColliderType::compositeColliderType;
        }

        ~CompositeCollider() {
        }

        void EarlyPhysicsUpdate(float dt) override {
        }

        void LatePhysicsUpdate(float dt) override {
            if (nullptr != positionProvider) {
                if (nullptr != orientationProvider) {
                    orientation = orientationProvider->GetOrientation();
                }
                position = positionProvider->GetPosition();
                bool isMinSet = false;
                bool isMaxSet = false;
                for (int i = 0; i < parts.size(); i++) {
                    parts[i]->SetPosition(orientation * positionsInOrigo[i] + position);
                    parts[i]->SetScale(scale);
                    parts[i]->SetOrientation(orientation);
                    parts[i]->LatePhysicsUpdate(dt);
                    glm::vec3 currentMin = parts[i]->GetAABBMin();
                    if (!isMinSet) {
                        aabbMin = currentMin;
                        isMinSet = true;
                    }
                    else {
                        if (currentMin.x < aabbMin.x) {
                            aabbMin.x = currentMin.x;
                        }
                        if (currentMin.y < aabbMin.y) {
                            aabbMin.y = currentMin.y;
                        }
                        if (currentMin.z < aabbMin.z) {
                            aabbMin.z = currentMin.z;
                        }
                    }
                    glm::vec3 currentMax = parts[i]->GetAABBMax();
                    if (!isMaxSet) {
                        aabbMax = currentMax;
                        isMaxSet = true;
                    }
                    else {
                        if (currentMax.x > aabbMax.x) {
                            aabbMax.x = currentMax.x;
                        }
                        if (currentMax.y > aabbMax.y) {
                            aabbMax.y = currentMax.y;
                        }
                        if (currentMax.z > aabbMax.z) {
                            aabbMax.z = currentMax.z;
                        }
                    }
                }
            }
        }

        // Inherited via Collider
        bool TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        bool TestPointInside(const glm::vec3& point) const override;

        bool CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool CollideWithSpherical(const Collider* collider) const override;

        bool CollideWithAABB(const Collider* collider) const override;

        bool CollideWithCuboid(const Collider* collider) const override;

        bool CollideWithComposite(const Collider* collider) const override;

        void addSubCollider(Collider* collider, const glm::vec3& positionInOrigo) {
            parts.push_back(collider);
            positionsInOrigo.push_back(glm::vec3(positionInOrigo));
            ExpandAABB(collider->GetAABBMin(), collider->GetAABBMax());
        }

        std::span<const Collider* const> getSubColliders() const {
            return parts;
        }

        // Inherited via Collider
        virtual glm::vec3 GetAABBMin() const override;

        virtual glm::vec3 GetAABBMax() const override;

    private:

        void ExpandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax) {
            glm::vec3 prevMin = aabbMin;
            glm::vec3 prevMax = aabbMax;
            glm::vec3 newMin;
            glm::vec3 newMax;
            newMin.x = (candidateMin.x < prevMin.x) ? candidateMin.x : prevMin.x;
            newMin.y = (candidateMin.y < prevMin.y) ? candidateMin.y : prevMin.y;
            newMin.z = (candidateMin.z < prevMin.z) ? candidateMin.z : prevMin.z;
            newMax.x = (candidateMax.x > prevMax.x) ? candidateMax.x : prevMax.x;
            newMax.y = (candidateMax.y > prevMax.y) ? candidateMax.y : prevMax.y;
            newMax.z = (candidateMax.z > prevMax.z) ? candidateMax.z : prevMax.z;
            aabbMin = newMin;
            aabbMax = newMax;
        }

        std::vector<Collider*> parts;
        std::vector<glm::vec3> positionsInOrigo;  // Reference positions of colliders parts

        bool IterateParts(std::function<bool(const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)>&& lambda,
            const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const;

        bool IterateParts(const std::function<bool(const Collider* collider1, const Collider* collider2)>&& lambda,
            const Collider* collider) const;

        glm::vec3 aabbMin;
        glm::vec3 aabbMax;

    };

}