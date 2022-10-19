#include "Collider.h"
#include "SceneEventManager.h"
#include "SceneEventImplementation.h"
#include "MemoryManager.h"
#include <iostream>
namespace Hogra {

    void Collider::Collide(Collider* collider)
    {
        auto& otherGroups = collider->GetColliderGroups();
        for (auto& otherGroup : otherGroups) {
            if (this->IsPartOfGroup(otherGroup)) {      // If two cilliders are part of the same group than no collision is calculated.
                return;
            }
        }
        bool isCollision = false;
        if (nullptr != physics) {
            glm::vec3 collisionPoint;
            glm::vec3 collisionNormal;
            float overlapAlongNormal;
            isCollision = TestCollision(collider, collisionPoint, collisionNormal, overlapAlongNormal);
            if (isCollision && nullptr != physics && nullptr != collider->GetPhysics()
                && !isnan(collisionPoint.x) && !isnan(collisionPoint.y) && !isnan(collisionPoint.z)
                && !isnan(collisionNormal.x) && !isnan(collisionNormal.y) && !isnan(collisionNormal.z)) {
                physics->Collide(*collider->GetPhysics(), collisionPoint, collisionNormal, overlapAlongNormal);
            }
        }
        else {
            isCollision = TestCollision(collider);
        }
        if (isCollision) {
            auto event = Allocator::New<CollisionEvent>();
            event->Init((const Collider*)this, collider);
            SceneEventManager::getInstance()->pushEvent(event);
            haveCollided = true;
            collider->SetHaveCollided(true);
        }

    }

    Physics* Collider::GetPhysics() const {
        return physics;
    }

    bool Collider::TestCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
    {
        bool isCollision = false;
        switch (collider->type)
        {
        case ColliderType::sphericalColliderType:
            isCollision = CollideWithSpherical(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            break;
        case ColliderType::AABBColliderType:
            isCollision = CollideWithAABB(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            break;
        case ColliderType::cuboidColliderType:
            isCollision = CollideWithCuboid(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            break;
        case ColliderType::compositeColliderType:
            isCollision = CollideWithComposite(collider, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
            break;
        default:
            break;
        }
        return isCollision;
    }

    bool Collider::TestCollision(const Collider* collider) const
    {
        bool isCollision = false;
        switch (collider->type)
        {
        case ColliderType::sphericalColliderType:
            isCollision = CollideWithSpherical(collider);
            break;
        case ColliderType::AABBColliderType:
            isCollision = CollideWithAABB(collider);
            break;
        case ColliderType::cuboidColliderType:
            isCollision = CollideWithCuboid(collider);
            break;
        case ColliderType::compositeColliderType:
            isCollision = CollideWithComposite(collider);
            break;
        default:
            break;
        }
        return isCollision;
    }

    void Collider::EarlyPhysicsUpdate(float dt)
    {
    }

    void Collider::LatePhysicsUpdate(float dt)
    {
        if (nullptr != positionProvider) {
            position = positionProvider->GetPosition();
        }
        if (nullptr != orientationProvider) {
            orientation = orientationProvider->GetOrientation();
        }
        if (nullptr != scaleProvider) {
            scale = scaleProvider->GetScale();
        }
    }

}