#pragma once
#include "Collider.h"
#include "Physics.h"

class SphericalCollider :
    public Collider
{
public:
    explicit SphericalCollider(Physics* physics) : Collider(), physics(physics) {}
    ~SphericalCollider() = default;

    // Inherited via Collider
    bool testRayIntersection(Ray& ray, glm::vec3& wIntersectionPoint) override;

    bool testCollision(Collider& collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;

protected:
    Physics* physics;
};

