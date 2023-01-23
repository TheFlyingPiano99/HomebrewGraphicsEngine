#include "SphericalCollider.h"

#include "AABBCollider.h"
#include "CompositeCollider.h"

namespace Hogra {

	bool SphericalCollider::TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const
	{
		glm::vec3 a = ray.GetPosition() - position;
		float b = glm::dot(a, ray.getDirection());
		if (float disc = b * b - (glm::dot(a, a) - radius * radius); 0.0f <= disc) {
			float t = -b - sqrtf(disc);
			if (t < 0.0f) {	// Ray start might be inside the sphere
				t = -b + sqrtf(disc);
			}
			if (t < 0.0f) {	// Sphere behind the ray start
				return false;
			}
			wIntersectionPoint = ray.GetPosition() + ray.getDirection() * t;
			wIntersectionNormal = glm::normalize(wIntersectionPoint - position);
			return true;
		}
		return false;
	}

	bool SphericalCollider::CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		const auto* sphericalCollider = (const SphericalCollider*)collider;
		glm::vec3 betweenCenters = sphericalCollider->GetPosition() - position;
		float l = glm::length(betweenCenters);
		bool isCollision = 0.0000001f < l && l <= radius + sphericalCollider->GetRadius();
		if (isCollision) {
			wCollisionPoint = position + betweenCenters / 2.0f;
			wCollisionNormal = glm::normalize(betweenCenters);
			overlapAlongNormal = radius + sphericalCollider->GetRadius() - l;
		}
		return isCollision;
	}

	bool SphericalCollider::CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		const auto* aabb = (const AABBCollider*)collider;
		glm::vec3 pos;
		pos.x = std::max(aabb->getMin().x, std::min(position.x, aabb->GetMax().x));
		pos.y = std::max(aabb->getMin().y, std::min(position.y, aabb->GetMax().y));
		pos.z = std::max(aabb->getMin().z, std::min(position.z, aabb->GetMax().z));
		float l = glm::length(pos - position);
		bool isCollision = l <= radius;
		if (isCollision) {
			wCollisionPoint = pos;
			wCollisionNormal = glm::normalize(pos - position);
			overlapAlongNormal = radius - l;
		}
		return isCollision;
	}

	bool SphericalCollider::CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		//TODO
		return false;
	}

	bool SphericalCollider::CollideWithSpherical(const Collider* collider) const
	{
		const auto* sphericalCollider = (const SphericalCollider*)collider;
		glm::vec3 betweenCenters = sphericalCollider->GetPosition() - position;
		float l = glm::length(betweenCenters);
		return l <= radius + sphericalCollider->GetRadius();
	}

	bool SphericalCollider::CollideWithAABB(const Collider* collider) const
	{
		const auto* aabb = (const AABBCollider*)collider;
		glm::vec3 pos;
		pos.x = std::max(aabb->getMin().x, std::min(position.x, aabb->GetMax().x));
		pos.y = std::max(aabb->getMin().y, std::min(position.y, aabb->GetMax().y));
		pos.z = std::max(aabb->getMin().z, std::min(position.z, aabb->GetMax().z));
		return glm::length(pos - position) <= radius;
	}

	bool SphericalCollider::CollideWithCuboid(const Collider* collider) const
	{
		//TODO
		return false;
	}

	bool SphericalCollider::CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const
	{
		bool isCollision = collider->CollideWithSpherical(this, wCollisionPoint, wCollisionNormal, overlapAlongNormal);
		wCollisionNormal *= -1.0f;
		return isCollision;
	}

	bool SphericalCollider::CollideWithComposite(const Collider* collider) const
	{
		return collider->CollideWithSpherical(this);
	}

	glm::vec3 SphericalCollider::GetAABBMin() const
	{
		return position - glm::vec3(radius, radius, radius);
	}

	glm::vec3 SphericalCollider::GetAABBMax() const
	{
		return position + glm::vec3(radius, radius, radius);
	}

	bool SphericalCollider::TestPointInside(const glm::vec3& point) const
	{
		return glm::length(point - position) <= radius;
	}

}