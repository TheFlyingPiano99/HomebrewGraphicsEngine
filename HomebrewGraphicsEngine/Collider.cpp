#include "Collider.h"

void Collider::collide(const Collider& collider)
{
    glm::vec3 collisionPoint;
    glm::vec3 collisionNormal;
    bool isCollision = testCollision(collider, collisionPoint, collisionNormal);
    if (isCollision && nullptr != physics && nullptr != collider.getPhysics()) {

    }
}

const Physics* Collider::getPhysics() const {
    return physics;
}
