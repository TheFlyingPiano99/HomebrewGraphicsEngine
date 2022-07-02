#include "SphericalCollider.h"

#include "AABBCollider.h"
#include "CompositeCollider.h"

namespace hograengine {

	bool SphericalCollider::testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
	{
		glm::vec3 a = ray.getPosition() - position;
		float b = glm::dot(a, ray.getDirection());
		if (float disc = b * b - (glm::dot(a, a) - radius * radius); 0.0f <= disc) {
			float t = -b - sqrtf(disc);
			if (t < 0.0f) {	// Ray start might be inside the sphere
				t = -b + sqrtf(disc);
			}
			if (t < 0.0f) {	// Sphere behind the ray start
				return false;
			}
			wIntersectionPoint = ray.getPosition() + ray.getDirection() * t;
			wIntersectionNormal = glm::normalize(wIntersectionPoint - position);
			return true;
		}
		return false;
	}

	bool SphericalCollider::collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		const auto* sphericalCollider = (const SphericalCollider*)collider;
		glm::vec3 betweenCenters = sphericalCollider->getPosition() - position;
		float l = glm::length(betweenCenters);
		bool isCollision = 0.0000001f < l && l <= radius + sphericalCollider->getRadius();
		if (isCollision) {
			wCollisionPoint = position + betweenCenters / 2.0f;
			wCollisionNormal = glm::normalize(betweenCenters);
			overlapAlongNormal = radius + sphericalCollider->getRadius() - l;
		}
		return isCollision;
	}

	bool SphericalCollider::collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		const auto* aabb = (const AABBCollider*)collider;
		glm::vec3 pos;
		pos.x = std::max(aabb->getMin().x, std::min(position.x, aabb->getMax().x));
		pos.y = std::max(aabb->getMin().y, std::min(position.y, aabb->getMax().y));
		pos.z = std::max(aabb->getMin().z, std::min(position.z, aabb->getMax().z));
		float l = glm::length(pos - position);
		bool isCollision = l <= radius;
		if (isCollision) {
			wCollisionPoint = pos;
			wCollisionNormal = glm::normalize(pos - position);
			overlapAlongNormal = radius - l;
		}
		return isCollision;
	}

	bool SphericalCollider::collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		//TODO
		return false;
	}

	bool SphericalCollider::collideWithSpherical(const Collider* collider) const
	{
		const auto* sphericalCollider = (const SphericalCollider*)collider;
		glm::vec3 betweenCenters = sphericalCollider->getPosition() - position;
		float l = glm::length(betweenCenters);
		return l <= radius + sphericalCollider->getRadius();
	}

	bool SphericalCollider::collideWithAABB(const Collider* collider) const
	{
		const auto* aabb = (const AABBCollider*)collider;
		glm::vec3 pos;
		pos.x = std::max(aabb->getMin().x, std::min(position.x, aabb->getMax().x));
		pos.y = std::max(aabb->getMin().y, std::min(position.y, aabb->getMax().y));
		pos.z = std::max(aabb->getMin().z, std::min(position.z, aabb->getMax().z));
		return glm::length(pos - position) <= radius;
	}

	bool SphericalCollider::collideWithCuboid(const Collider* collider) const
	{
		//TODO
		return false;
	}

	bool SphericalCollider::collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		return collider->collideWithSpherical(this, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
	}

	bool SphericalCollider::collideWithComposite(const Collider* collider) const
	{
		return collider->collideWithSpherical(this);
	}

	glm::vec3 SphericalCollider::getAABBMin()
	{
		return position - glm::vec3(radius, radius, radius);
	}

	glm::vec3 SphericalCollider::getAABBMax()
	{
		return position + glm::vec3(radius, radius, radius);
	}

	bool SphericalCollider::testPointInside(const glm::vec3& point) const
	{
		return glm::length(point - position) <= radius;
	}

}