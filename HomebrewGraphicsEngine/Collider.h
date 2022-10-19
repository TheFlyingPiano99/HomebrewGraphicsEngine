#pragma once
#include "Ray.h"
#include "Physics.h"
#include "CollisionEventListener.h"
#include <set>

namespace Hogra {

	/*
	* Virtual base class of all colliders.
	*/
	class Collider : public Component
	{
	public:

		/*
		* Called from scene.
		*/
		void Collide(Collider* collider);

		Physics* GetPhysics() const;

		void SetPhysics(Physics* p) {
			physics = p;
		}

		bool TestCollision(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const;

		/*
		* Cheaper version, without collision point and normal calculation t/f output
		*/
		bool TestCollision(const Collider* collider) const;

		virtual bool TestRayIntersection(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal) const = 0;

		virtual bool TestPointInside(const glm::vec3& point) const = 0;

		enum class ColliderType {
			sphericalColliderType,
			AABBColliderType,
			cuboidColliderType,
			compositeColliderType,
			undefinedColliderType
		};
		ColliderType type = ColliderType::undefinedColliderType;

		// Inherited via Component
		virtual void EarlyPhysicsUpdate(float dt) override;

		virtual void LatePhysicsUpdate(float dt) override;

		const glm::vec3& GetPosition() const {
			return position;
		}

		void SetPosition(const glm::vec3& p) {
			position = p;
		}

		const glm::vec3& GetScale() const {
			return scale;
		}

		void SetScale(const glm::vec3& s) {
			scale = s;
		}

		const glm::quat& GetOrientation() const {
			return orientation;
		}

		void SetOrientation(const glm::quat& o) {
			orientation = o;
		}

		void AddToColliderGroup(int group) {
			auto iter = std::find(colliderGroups.begin(), colliderGroups.end(), group);
			if (colliderGroups.end() == iter) {
				colliderGroups.push_back(group);
			}
		}

		void RemoveFromColliderGroup(int group) {
			auto iter = std::find(colliderGroups.begin(), colliderGroups.end(), group);
			if (colliderGroups.end() != iter) {
				colliderGroups.erase(iter);
			}
		}

		bool IsPartOfGroup(int group) const {
			return colliderGroups.end() != std::find(colliderGroups.begin(), colliderGroups.end(), group);
		}

		const std::vector<int>& GetColliderGroups() const {
			return colliderGroups;
		}

		virtual bool CollideWithSpherical(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool CollideWithAABB(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool CollideWithCuboid(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;
		virtual bool CollideWithComposite(const Collider* collider, glm::vec3& wCollisionPoint, glm::vec3& wCollisionNormal, float& overlapAlongNormal) const = 0;

		virtual bool CollideWithSpherical(const Collider* collider) const = 0;
		virtual bool CollideWithAABB(const Collider* collider) const = 0;
		virtual bool CollideWithCuboid(const Collider* collider) const = 0;
		virtual bool CollideWithComposite(const Collider* collider) const = 0;
	
		// For collision optimalisation:
		virtual glm::vec3 GetAABBMin() const = 0;
		virtual glm::vec3 GetAABBMax() const = 0;

		bool PopCollided() {
			bool temp = haveCollided;
			haveCollided = false;
			return temp;
		}

		void SetHaveCollided(bool b) {
			haveCollided = b;
		}

		void SetPositionProvider(const PositionProvider* provider) {
			positionProvider = provider;
		}

		void SetOrientationProvider(const OrientationProvider* provider) {
			orientationProvider = provider;
		}

		void SetScaleProvoder(const ScaleProvider* provider) {
			scaleProvider = provider;
		}

	protected:
		Physics* physics = nullptr;
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::quat orientation = angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 position = glm::vec3(0.0f);

		const PositionProvider* positionProvider = nullptr;
		const OrientationProvider* orientationProvider = nullptr;
		const ScaleProvider* scaleProvider = nullptr;

		std::vector<int> colliderGroups; // The list of collider groups that this collider is part of.
		bool haveCollided = false;
	};
}
