#include "AABBCollider.h"

#include "SphericalCollider.h"
namespace hograengine {

    bool AABBCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
    {
        float tmin = (min.x - ray.getPosition().x) / ray.getDirection().x;
        float tmax = (max.x - ray.getPosition().x) / ray.getDirection().x;
        float normalX = -1.0f;
        if (tmin > tmax) {
            std::swap(tmin, tmax);
            normalX = 1.0f;
        }
        float tymin = (min.y - ray.getPosition().y) / ray.getDirection().y;
        float tymax = (max.y - ray.getPosition().y) / ray.getDirection().y;
        float normalY = -1.0f;
        if (tymin > tymax) {
            std::swap(tymin, tymax);
            normalY = 1.0f;
        }
        glm::vec3 n = glm::vec3(normalX, 0.0f, 0.0f);
        if ((tmin > tymax) || (tymin > tmax)) {
            return false;
        }
        if (tymin > tmin) {
            tmin = tymin;
            n = glm::vec3(0.0f, normalY, 0.0f);
        }
        if (tymax < tmax) {
            tmax = tymax;
        }
        float tzmin = (min.z - ray.getPosition().z) / ray.getDirection().z;
        float tzmax = (max.z - ray.getPosition().z) / ray.getDirection().z;
        float normalZ = -1.0f;
        if (tzmin > tzmax) {
            std::swap(tzmin, tzmax);
            normalZ = 1.0f;
        }

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin) {
            tmin = tzmin;
            n = glm::vec3(0.0f, 0.0f, normalZ);
        }

        if (tzmax < tmax) {
            tmax = tzmax;
        }

        wIntersectionPoint = ray.getPosition() + ray.getDirection() * tmin;
        wIntersectionNormal = n;

        return true;
    }

    bool AABBCollider::testPointInside(const glm::vec3& point) const
    {
        return point.x >= min.x && point.x <= max.x
            && point.y >= min.y && point.y <= max.y
            && point.z >= min.z && point.z <= max.z;
    }

    bool AABBCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        const auto* spherical = (const SphericalCollider*)collider;
        glm::vec3 pos;
        pos.x = std::max(min.x, std::min(spherical->getPosition().x, max.x));
        pos.y = std::max(min.y, std::min(spherical->getPosition().y, max.y));
        pos.z = std::max(min.z, std::min(spherical->getPosition().z, max.z));
        float l = glm::length(pos - spherical->getPosition());
        bool isCollision = l <= spherical->getRadius();
        if (isCollision) {
            wCollisionPoint = pos;
            wCollisionNormal = glm::normalize(spherical->getPosition() - pos);
            overlapAlongNormal = spherical->getRadius() - l;
        }
        return isCollision;
    }

    bool AABBCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
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

            glm::vec3 nearestDist;
            glm::vec3 dir;
            if (wCollisionPoint.x - min.x < max.x - wCollisionPoint.x) {
                nearestDist.x = wCollisionPoint.x - min.x;
                dir.x = -1.0f;
            }
            else {
                nearestDist.x = max.x - wCollisionPoint.x;
                dir.x = 1.0f;
            }
            if (wCollisionPoint.y - min.y < max.y - wCollisionPoint.y) {
                nearestDist.y = wCollisionPoint.y - min.y;
                dir.y = -1.0f;
            }
            else {
                nearestDist.y = max.y - wCollisionPoint.y;
                dir.y = 1.0f;
            }
            if (wCollisionPoint.z - min.z < max.z - wCollisionPoint.z) {
                nearestDist.z = wCollisionPoint.z - min.z;
                dir.z = -1.0f;
            }
            else {
                nearestDist.z = max.z - wCollisionPoint.z;
                dir.z = 1.0f;
            }
            if (nearestDist.x < nearestDist.y) {
                if (nearestDist.x < nearestDist.z) {
                    wCollisionNormal = glm::vec3(dir.x, 0.0f, 0.0f);
                }
                else {
                    wCollisionNormal = glm::vec3(0.0f, 0.0f, dir.z);
                }
            }
            else {
                if (nearestDist.y < nearestDist.z) {
                    wCollisionNormal = glm::vec3(0.0f, dir.y, 0.0f);
                }
                else {
                    wCollisionNormal = glm::vec3(0.0f, 0.0f, dir.z);
                }
            }
            glm::vec3 intersectionScale = intesectionMax - intesectionMin;
            overlapAlongNormal = abs(intersectionScale.x * wCollisionNormal.x + intersectionScale.y * wCollisionNormal.y + intersectionScale.z * wCollisionNormal.z);
        }
        return isCollision;
    }

    bool AABBCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        //TODO
        return false;
    }

    bool AABBCollider::collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        bool isCollision = collider->collideWithAABB(this, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
        wCollisionNormal *= -1.0f;
        return isCollision;
    }

    // Simple collisions: -------------------------------------------------------------------

    bool AABBCollider::collideWithSpherical(const Collider* collider) const
    {
        const auto* spherical = (const SphericalCollider*)collider;
        glm::vec3 pos;
        pos.x = std::max(min.x, std::min(spherical->getPosition().x, max.x));
        pos.y = std::max(min.y, std::min(spherical->getPosition().y, max.y));
        pos.z = std::max(min.z, std::min(spherical->getPosition().z, max.z));
        return glm::length(pos - spherical->getPosition()) <= spherical->getRadius();
    }

    bool AABBCollider::collideWithAABB(const Collider* collider) const
    {
        const auto* aabb = (const AABBCollider*)collider;
        return aabb->getMin().x <= max.x && aabb->getMax().x >= min.x
            && aabb->getMin().y <= max.y && aabb->getMax().y >= min.y
            && aabb->getMin().z <= max.z && aabb->getMax().z >= min.z;
    }

    bool AABBCollider::collideWithCuboid(const Collider* collider) const
    {
        //TODO
        return false;
    }

    bool AABBCollider::collideWithComposite(const Collider* collider) const
    {
        return collider->collideWithAABB(this);
    }

    glm::vec3 AABBCollider::getAABBMin() const
    {
        return position + minInOrigo;
    }

    glm::vec3 AABBCollider::getAABBMax() const
    {
        return position + maxInOrigo;
    }

}