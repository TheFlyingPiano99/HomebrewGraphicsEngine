#include "CompositeCollider.h"
#include <algorithm>
namespace Hogra {

    bool CompositeCollider::TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
    {
        float minT = -1.0f;
        glm::vec3 point = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        for (auto* collider : parts) {
            if (collider->TestRayIntersection(ray, wIntersectionPoint, wIntersectionNormal)) {
                float t = glm::length(ray.GetPosition() - wIntersectionPoint);
                if (t < minT || minT < 0.0f) {
                    minT = t;
                    point = wIntersectionPoint;
                    normal = wIntersectionNormal;
                }
            }
        }
        wIntersectionPoint = point;
        wIntersectionNormal = normal;
        return (minT >= 0.0f);
    }

    bool CompositeCollider::TestPointInside(const glm::vec3& point) const
    {
        for (auto* collider : parts) {
            if (collider->TestPointInside(point)) {
                return true;
            }
        }
        return false;
    }

    bool CompositeCollider::CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return IterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->CollideWithSpherical(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return IterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->CollideWithAABB(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return IterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->CollideWithCuboid(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::CollideWithSpherical(const Collider* collider) const
    {
        return IterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->CollideWithSpherical(collider2);
            },
            collider);
    }

    bool CompositeCollider::CollideWithAABB(const Collider* collider) const
    {
        return IterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->CollideWithAABB(collider2);
            },
            collider);
    }

    bool CompositeCollider::CollideWithCuboid(const Collider* collider) const
    {
        return IterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->CollideWithCuboid(collider2);
            },
            collider);
    }

    bool CompositeCollider::IterateParts(std::function<bool(const Collider* collider1, const Collider* collider2, glm::vec3&, glm::vec3&, float&)>&& lambda, const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        bool isCollision = false;
        glm::vec3 sumPoint = glm::vec3(0.0f);
        glm::vec3 sumNormal = glm::vec3(0.0f);
        float sumOverlap = 0.0f;
        unsigned int noOfIntersect = 0;
        for (auto& c : parts) {
            glm::vec3 point;
            glm::vec3 normal;
            float overlap;
            if (lambda(c, collider, point, normal, overlap)) {
                isCollision = true;
                sumPoint += point;
                sumNormal += normal;
                sumOverlap += overlap;
                noOfIntersect++;
            }
        }
        if (isCollision) {
            wCollisionPoint = sumPoint / (float)noOfIntersect;
            wCollisionNormal = glm::normalize(sumNormal);
            overlapAlongNormal = sumOverlap / (float)noOfIntersect;
        }
        return isCollision;
    }

    bool CompositeCollider::IterateParts(const std::function<bool(const Collider* collider1, const Collider* collider2)>&& lambda, const Collider* collider) const
    {
        for (auto& c : parts) {
            if (lambda(c, collider)) {
                return true;
            }
        }
        return false;
    }

    glm::vec3 CompositeCollider::GetAABBMin() const
    {
        return aabbMin;
    }

    glm::vec3 CompositeCollider::GetAABBMax() const
    {
        return aabbMax;
    }

    bool CompositeCollider::CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return IterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->CollideWithComposite(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::CollideWithComposite(const Collider* collider) const
    {
        return IterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->CollideWithComposite(collider2);
            },
            collider);
    }
}