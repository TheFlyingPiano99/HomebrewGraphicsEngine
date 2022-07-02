#pragma once
#include "Collider.h"
#include <functional>

namespace hograengine {

    class CompositeCollider :
        public Collider
    {
    public:
        CompositeCollider(Physics* physics = nullptr) : Collider(physics) {
            type = ColliderType::compositeColliderType;
        }

        ~CompositeCollider() {
            for (auto& collider : parts) {
                delete collider;
            }
            for (auto& pos : positionsInOrigo) {
                delete pos;
            }
        }

        void control(float dt) override {
        }

        void update(float dt) override {
            if (nullptr != physics) {
                position = physics->getOwnerPosition();
                scale = physics->getOwnerScale();
                orientation = physics->getOwnerOrientation();
                bool isMinSet = false;
                bool isMaxSet = false;
                for (int i = 0; i < parts.size(); i++) {
                    parts[i]->setPosition(orientation * *positionsInOrigo[i] + position);
                    parts[i]->setScale(scale);
                    parts[i]->setOrientation(orientation);
                    parts[i]->update(dt);
                    glm::vec3 currentMin = parts[i]->getAABBMin();
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
                    glm::vec3 currentMax = parts[i]->getAABBMax();
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
        bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

        bool testPointInside(const glm::vec3& point) const override;

        bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

        bool collideWithSpherical(const Collider* collider) const override;

        bool collideWithAABB(const Collider* collider) const override;

        bool collideWithCuboid(const Collider* collider) const override;

        bool collideWithComposite(const Collider* collider) const override;

        void addSubCollider(Collider* collider, const glm::vec3& positionInOrigo) {
            parts.push_back(collider);
            positionsInOrigo.push_back(new glm::vec3(positionInOrigo));
            expandAABB(collider->getAABBMin(), collider->getAABBMax());
        }

        std::span<const Collider* const> getSubColliders() const {
            return parts;
        }

        // Inherited via Collider
        virtual glm::vec3 getAABBMin() override;

        virtual glm::vec3 getAABBMax() override;

    private:

        void expandAABB(const glm::vec3& candidateMin, const glm::vec3& candidateMax) {
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
        std::vector<glm::vec3*> positionsInOrigo;  // Reference positions of colliders parts

        bool iterateParts(std::function<bool(const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)>&& lambda,
            const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const;

        bool iterateParts(const std::function<bool(const Collider* collider1, const Collider* collider2)>&& lambda,
            const Collider* collider) const;

        glm::vec3 aabbMin;
        glm::vec3 aabbMax;

    };

}