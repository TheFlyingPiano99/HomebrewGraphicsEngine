#pragma once
#include "Ray.h"
#include "Physics.h"
#include "CollisionEventListener.h"

namespace hograengine {

	/*
	* Virtual base class of all colliders.
	*/
	class Collider : public Component
	{
	public:

		Collider(Physics* physics = nullptr) : physics(physics) {}

		/*
		* Called from scene.
		*/
		void collide(const Collider& collider) const;

		Physics* getPhysics() const;

		void setPhysics(Physics* p) {
			physics = p;
		}

		float getElasticity() const {
			return elasticity;
		}

		void setElasticity(float e) {
			elasticity = e;
		}

		bool testCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const;

		/*
		* Cheaper version, without collision point and normal calculation t/f output
		*/
		bool testCollision(const Collider* collider) const;

		virtual bool testRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const = 0;

		virtual bool testPointInside(const glm::vec3& point) const = 0;

		enum class ColliderType {
			sphericalColliderType,
			AABBColliderType,
			cuboidColliderType,
			compositeColliderType,
			undefinedColliderType
		};
		ColliderType type = ColliderType::undefinedColliderType;

		// Inherited via Component
		virtual void control(float dt) override;

		virtual void update(float dt) override;

		const glm::vec3& getPosition() const {
			return position;
		}

		void setPosition(const glm::vec3& p) {
			position = p;
		}

		const glm::vec3& getScale() const {
			return scale;
		}

		void setScale(const glm::vec3& s) {
			scale = s;
		}

		const glm::quat& getOrientation() const {
			return orientation;
		}

		void setOrientation(const glm::quat& o) {
			orientation = o;
		}

		virtual bool collideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool collideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool collideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool collideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;

		virtual bool collideWithSpherical(const Collider* collider) const = 0;
		virtual bool collideWithAABB(const Collider* collider) const = 0;
		virtual bool collideWithCuboid(const Collider* collider) const = 0;
		virtual bool collideWithComposite(const Collider* collider) const = 0;

	protected:

		Physics* physics = nullptr;
		float elasticity = 1.0f;	// From [0..1] interval: 1 ~ fully elastic; 0 ~ inelastic. 
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::quat orientation = angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 position = glm::vec3(0.0f);
	};
}
