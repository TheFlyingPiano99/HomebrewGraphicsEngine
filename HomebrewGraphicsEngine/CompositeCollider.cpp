#include "CompositeCollider.h"
#include <algorithm>
namespace hograengine {

    bool CompositeCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
    {
        float minT = -1.0f;
        glm::vec3 point = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        for (auto* collider : parts) {
            if (collider->testRayIntersection(ray, wIntersectionPoint, wIntersectionNormal)) {
                float t = glm::length(ray.getPosition() - wIntersectionPoint);
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

    bool CompositeCollider::testPointInside(const glm::vec3& point) const
    {
        for (auto* collider : parts) {
            if (collider->testPointInside(point)) {
                return true;
            }
        }
        return false;
    }

    bool CompositeCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return iterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->collideWithSpherical(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return iterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->collideWithAABB(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return iterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->collideWithCuboid(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::collideWithSpherical(const Collider* collider) const
    {
        return iterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->collideWithSpherical(collider2);
            },
            collider);
    }

    bool CompositeCollider::collideWithAABB(const Collider* collider) const
    {
        for (auto* c : parts) {
            c->collide(collider);
        }
        return false;
    }

    bool CompositeCollider::collideWithCuboid(const Collider* collider) const
    {
        for (auto* c : parts) {
            c->collide(collider);
        }
        return false;
    }

    bool CompositeCollider::iterateParts(std::function<bool(const Collider* collider1, const Collider* collider2, glm::vec3&, glm::vec3&, float&)>&& lambda, const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
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

    bool CompositeCollider::iterateParts(const std::function<bool(const Collider* collider1, const Collider* collider2)>&& lambda, const Collider* collider) const
    {
        for (auto& c : parts) {
            if (lambda(c, collider)) {
                return true;
            }
        }
        return false;
    }

    glm::vec3 CompositeCollider::getAABBMin()
    {
        return aabbMin;
    }

    glm::vec3 CompositeCollider::getAABBMax()
    {
        return aabbMax;
    }

    bool CompositeCollider::collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        return iterateParts([](const Collider* collider1, const Collider* collider2, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) {
            return collider1->collideWithComposite(collider2, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            }, collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
    }

    bool CompositeCollider::collideWithComposite(const Collider* collider) const
    {
        return iterateParts(
            [](const Collider* collider1, const Collider* collider2) {
                return collider1->collideWithComposite(collider2);
            },
            collider);
    }
}