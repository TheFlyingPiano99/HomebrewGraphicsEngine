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
            if (nullptr != physics) {
                position = physics->getOwnerPosition();
                scale = physics->getOwnerScale();
                orientation = physics->getOwnerOrientation();

                for (int i = 0; i < parts.size(); i++) {
                    parts[i]->setPosition(orientation * *positionsInOrigo[i] + position);
                    parts[i]->setScale(scale);
                    parts[i]->setOrientation(orientation);
                    parts[i]->update(dt);
                }
            }
        }

        void update(float dt) override {
            if (nullptr != physics) {
                position = physics->getOwnerPosition();
                scale = physics->getOwnerScale();
                orientation = physics->getOwnerOrientation();

                for (int i = 0; i < parts.size(); i++) {
                    parts[i]->setPosition(orientation * *positionsInOrigo[i] + position);
                    parts[i]->setScale(scale);
                    parts[i]->setOrientation(orientation);
                    parts[i]->update(dt);
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
        }

        std::span<const Collider* const> getSubColliders() const {
            return parts;
        }

    private:
        std::vector<Collider*> parts;
        std::vector<glm::vec3*> positionsInOrigo;  // Reference positions of colliders parts

        bool iterateParts(std::function<bool(const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal)>&& lambda,
            const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const;

        bool iterateParts(const std::function<bool(const Collider* collider1, const Collider* collider2)>&& lambda,
            const Collider* collider) const;

    };

}