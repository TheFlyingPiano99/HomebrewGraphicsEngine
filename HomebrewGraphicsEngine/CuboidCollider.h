#pragma once
#include "Collider.h"
class CuboidCollider :
    public Collider
{
public:

    // Inherited via Collider
    virtual bool testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal) override;

    virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) override;

    virtual void control(float dt) override;

    virtual void update(float dt) override;

protected:

};

