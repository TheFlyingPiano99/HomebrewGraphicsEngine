#include "AABBCollider.h"

#include "SphericalCollider.h"

bool AABBCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal)
{
    return false;
}

bool AABBCollider::testPointInside(const glm::vec3& point)
{
    return point.x >= min.x && point.x <= max.x
        && point.y >= min.y && point.y <= max.y
        && point.z >= min.z && point.z <= max.z;
}

bool AABBCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
    const auto* spherical = (const SphericalCollider*)collider;
    glm::vec3 pos;
    pos.x = std::max(min.x, std::min(spherical->getPosition().x, max.x));
    pos.y = std::max(min.y, std::min(spherical->getPosition().y, max.y));
    pos.z = std::max(min.z, std::min(spherical->getPosition().z, max.z));
    bool isCollision = glm::length(pos - spherical->getPosition()) <= spherical->getRadius();
    if (isCollision) {
        wCollisionPoint = pos;
        wCollisionNormal = glm::normalize(spherical->getPosition() - pos);
    }
    return isCollision;
}

bool AABBCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
    const auto* aabb = (const AABBCollider*)collider;
    bool isCollision = aabb->getMin().x <= max.x && aabb->getMax().x >= min.x
                    && aabb->getMin().y <= max.y && aabb->getMax().y >= min.y
                    && aabb->getMin().z <= max.z && aabb->getMax().z >= min.z;
    if (isCollision) {
        glm::vec3 intesectionMin;
        glm::vec3 intesectionMax;
        intesectionMin.x = std::max(min.x, aabb->getMin().x);
        intesectionMin.y = std::max(min.y, aabb->getMin().y);
        intesectionMin.z = std::max(min.z, aabb->getMin().z);
        intesectionMax.x = std::min(max.x, aabb->getMax().x);
        intesectionMax.y = std::min(max.y, aabb->getMax().y);
        intesectionMax.z = std::min(max.z, aabb->getMax().z);
        wCollisionPoint = (intesectionMin + intesectionMax) / 2.0f;
        wCollisionNormal = glm::normalize(intesectionMin - min);
    }
    return isCollision;
}

bool AABBCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal)
{
    //TODO
    return false;
}

bool AABBCollider::collideWithSpherical(const Collider* collider)
{
    const auto* spherical = (const SphericalCollider*)collider;
    glm::vec3 pos;
    pos.x = std::max(min.x, std::min(spherical->getPosition().x, max.x));
    pos.y = std::max(min.y, std::min(spherical->getPosition().y, max.y));
    pos.z = std::max(min.z, std::min(spherical->getPosition().z, max.z));
    return glm::length(pos - spherical->getPosition()) <= spherical->getRadius();
}

bool AABBCollider::collideWithAABB(const Collider* collider)
{
    const auto* aabb = (const AABBCollider*)collider;
    return aabb->getMin().x <= max.x && aabb->getMax().x >= min.x
        && aabb->getMin().y <= max.y && aabb->getMax().y >= min.y
        && aabb->getMin().z <= max.z && aabb->getMax().z >= min.z;
}

bool AABBCollider::collideWithCuboid(const Collider* collider)
{
    //TODO
    return false;
}
