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
    bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const override;

    bool testPointInside(const glm::vec3& point) const override;

    float getRadius() const {
        return radius;
    }

    void setRadius(float r) {
        radius = r;
    }

private:
    float radius;

    bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;
    bool collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const override;

    bool collideWithSpherical(const Collider* collider) const override;
    bool collideWithAABB(const Collider* collider) const override;
    bool collideWithCuboid(const Collider* collider) const override;
    bool collideWithComposite(const Collider* collider) const override;

};

