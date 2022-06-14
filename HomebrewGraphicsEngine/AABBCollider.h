#pragma once
#include "Collider.h"

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

    virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) override;

    virtual bool testPointInside(const glm::vec3& point) override;

    void control(float dt) override {
        if (nullptr != physics) {
            position = physics->getOwnerPosition();
            scale = physics->getOwnerScale();
            orientation = physics->getOwnerOrientation();
            glm::vec3 pos0 = orientation * (scale * glm::vec3(-1.0f, -1.0f, -1.0f)) + position;
            glm::vec3 pos1 = orientation * (scale * glm::vec3(1.0f, 1.0f, 1.0f)) + position;
            min.x = std::min(pos0.x, pos1.x);
            min.y = std::min(pos0.y, pos1.y);
            min.z = std::min(pos0.z, pos1.z);
            max.x = std::max(pos0.x, pos1.x);
            max.y = std::max(pos0.y, pos1.y);
            max.z = std::max(pos0.z, pos1.z);
        }
    }

    void update(float dt) override {
        if (nullptr != physics) {
            position = physics->getOwnerPosition();
            scale = physics->getOwnerScale();
            orientation = physics->getOwnerOrientation();
            glm::vec3 pos0 = orientation * scale * glm::vec3(-1.0f, -1.0f, -1.0f) + position;
            glm::vec3 pos1 = orientation * scale * glm::vec3(1.0f, 1.0f, 1.0f) + position;
            min.x = std::min(pos0.x, pos1.x);
            min.y = std::min(pos0.y, pos1.y);
            min.z = std::min(pos0.z, pos1.z);
            max.x = std::max(pos0.x, pos1.x);
            max.y = std::max(pos0.y, pos1.y);
            max.z = std::max(pos0.z, pos1.z);
        }
    }

    const glm::vec3& getMin() const {
        return min;
    }

    void setMin(const glm::vec3& _min) {
        min = _min;
    }

    const glm::vec3& getMax() const {
        return max;
    }

    void setMax(const glm::vec3& _max) {
        max = _max;
    }

private:
    glm::vec3 min = glm::vec3();
    glm::vec3 max = glm::vec3();

    bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) override;
    bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) override;
    bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) override;

    bool collideWithSpherical(const Collider* collider) override;
    bool collideWithAABB(const Collider* collider) override;
    bool collideWithCuboid(const Collider* collider) override;
};

