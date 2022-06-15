#include "Collider.h"
#include "SceneEventManager.h"
#include "SceneEventImplementation.h"
#include <iostream>

void Collider::collide(const Collider& collider) const
{
    bool isCollision = false;
    if (nullptr != physics) {
        glm::vec3 collisionPoint;
        glm::vec3 collisionNormal;
        float overlapAlongNormal;
        isCollision = testCollision(&collider, collisionPoint, collisionNormal, overlapAlongNormal);
        if (isCollision && nullptr != physics && nullptr != collider.getPhysics()
            && !isnan(collisionPoint.x) && !isnan(collisionPoint.y) && !isnan(collisionPoint.z)
            && !isnan(collisionNormal.x) && !isnan(collisionNormal.y) && !isnan(collisionNormal.z)) {
            physics->collide(*collider.getPhysics(), collisionPoint, collisionNormal, overlapAlongNormal, elasticity * collider.getElasticity());
        }
    }
    else {
        isCollision = testCollision(&collider);
    }
    if (isCollision) {
        SceneEventManager::getInstance()->pushEvent(new CollisionEvent((const Collider*)this, &collider));
    }

}

Physics* Collider::getPhysics() const {
    return physics;
}

bool Collider::testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
{
    bool isCollision = false;
    switch (collider->type)
    {
    case ColliderType::sphericalColliderType:
        isCollision = collideWithSpherical(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
        break;
    case ColliderType::AABBColliderType:
        isCollision = collideWithAABB(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
        break;
    case ColliderType::cuboidColliderType:
        isCollision = collideWithCuboid(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
        break;
    case ColliderType::compositeColliderType:
        isCollision = collideWithComposite(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
        break;
    default:
        break;
    }
    return isCollision;
}

bool Collider::testCollision(const Collider* collider) const
{
    bool isCollision = false;
    switch (collider->type)
    {
    case ColliderType::sphericalColliderType:
        isCollision = collideWithSpherical(collider);
        break;
    case ColliderType::AABBColliderType:
        isCollision = collideWithAABB(collider);
        break;
    case ColliderType::cuboidColliderType:
        isCollision = collideWithCuboid(collider);
        break;
    default:
        break;
    }
    return isCollision;
}

void Collider::control(float dt)
{
    if (nullptr != physics) {
        position = physics->getOwnerPosition();
        scale = physics->getOwnerScale();
        orientation = physics->getOwnerOrientation();
    }
}

void Collider::update(float dt)
{
    if (nullptr != physics) {
        position = physics->getOwnerPosition();
        scale = physics->getOwnerScale();
        orientation = physics->getOwnerOrientation();
    }
}
