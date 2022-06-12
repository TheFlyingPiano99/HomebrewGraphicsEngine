#pragma once
#include "Collider.h"

class SphericalCollider :
    public Collider
{
public:
    explicit SphericalCollider(Physics* physics) : Collider(physics) {}
    ~SphericalCollider() = default;

    // Inherited via Collider
    bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint) override;

    bool testCollision(const Collider& collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;

};

