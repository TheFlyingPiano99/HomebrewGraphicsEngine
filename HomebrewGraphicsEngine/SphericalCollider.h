#pragma once
#include "Collider.h"

class SphericalCollider :
    public Collider
{
public:
    explicit SphericalCollider(Physics* physics) : Collider(physics) {
        type = ColliderType::sphericalColliderType;
    }
    ~SphericalCollider() = default;

    // Inherited via Collider
    bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) override;

    bool testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;

    float getRadius() const {
        return radius;
    }

    void setRadius(float r) {
        radius = r;
    }

private:
    float radius;

    bool colliderWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal);
};

