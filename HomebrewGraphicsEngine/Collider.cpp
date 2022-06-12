#include "Collider.h"

void Collider::collide(const Collider& collider)
{
    glm::vec3 collisionPoint;
    glm::vec3 collisionNormal;
    bool isCollision = testCollision(&collider, collisionPoint, collisionNormal);
    if (isCollision && nullptr != physics && nullptr != collider.getPhysics()) {
        physics->collide(*collider.getPhysics(), collisionPoint, collisionNormal, elasticity * collider.getElasticity());
    }
}

Physics* Collider::getPhysics() const {
    return physics;
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
