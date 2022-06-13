#pragma once
#include "Collider.h"

/*
* Simple and cheap sphere shaped collider object.
*/
class SphericalCollider :
    public Collider
{
public:
    explicit SphericalCollider(Physics* physics = nullptr) : Collider(physics) {
        type = ColliderType::sphericalColliderType;
    }
    ~SphericalCollider() = default;

    // Inherited via Collider
    bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) override;

    bool testPointInside(const glm::vec3& point) override;

    float getRadius() const {
        return radius;
    }

    void setRadius(float r) {
        radius = r;
    }

private:
    float radius;

    bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;
    bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;
    bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;

    bool collideWithSpherical(const Collider* collider) override;
    bool collideWithAABB(const Collider* collider) override;
    bool collideWithCuboid(const Collider* collider) override;
};

