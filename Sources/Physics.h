#pragma once
#include "Component.h"
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "DebugUtils.h"

namespace Hogra {

	class Physics : public Component
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		void Init(SceneObject* _owner) {
			owner = _owner;
		}
		
		~Physics() override = default;

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt_sec) override;
		void LatePhysicsUpdate(float dt_sec) override;

		/*
		* return force applied to center of mass in Newtons
		*/
		glm::vec3 GetAppliedForce() const {
			return appliedForce;
		}

		/*
		* set force applied to center of mass in Newtons
		*/
		void SetAppliedForce(const glm::vec3& force) {
			appliedForce = force;
		}

		/*
		* add force applied to center of mass in Newtons
		*/
		void AddAppliedForce(const glm::vec3& additionalForce) {
			appliedForce += additionalForce;
		}

		void ApplyImpulse(const glm::vec3& _impulse, const glm::vec3& momentArm) {
			impulse += _impulse;
			impulseAsIntegratedTorque += glm::cross(momentArm, _impulse);
		}

		void ApplyTransientForce(const glm::vec3& force) {
			appliedTransientForce += force;
		}

		void ApplyTransientTorque(const glm::vec3& torque) {
			appliedTransientTorque += torque;
		}

		/*
		* get momentum [kg * m / s]
		*/
		glm::vec3 GetMomentum() const {
			return momentum;
		}

		/*
		* set momentum [kg * m / s]
		*/
		void SetMomentum(const glm::vec3& p) {
			momentum = p;
		}

		/*
		* get velocity in m/s
		*/
		glm::vec3 GetVelocity() const {
			return momentum * invMass_1_per_kg;
		}

		/*
		* set velocity in m/s
		*/
		void SetVelocity(const glm::vec3& v) {
			momentum = v / invMass_1_per_kg;
		}

		/*
		* get mass in kg
		*/
		float GetMass() const {
			return 1.0f / invMass_1_per_kg;
		}

		/*
		* get inverse of mass in 1/kg
		*/
		float GetInvMass() const {
			return invMass_1_per_kg;
		}

		/*
		* set mass in kg
		*/
		void SetMass(const float m) {
			if (m == 0.0) {
				DebugUtils::PrintError("Physics", "Trying to set 0 mass. Division by 0!");
				return;
			}
			invMass_1_per_kg = 1.0f / std::fabsf(m);
		}

		glm::vec3 GetModelSpaceDrag() const {
			return modelSpaceDrag;
		}

		void SetModelSpaceDrag(const glm::vec3& mDrag) {
			modelSpaceDrag = mDrag;
		}

		glm::vec3 GetWorldSpaceDrag() const {
			return worldSpaceDrag;
		}

		void SetWorldSpaceDrag(const glm::vec3& wDrag) {
			worldSpaceDrag = wDrag;
		}

		glm::vec3 GetAppliedTorque() const {
			return appliedTorque;
		}

		void SetAppliedTorque(const glm::vec3& torque) {
			appliedTorque = torque;
		}

		void AddAppliedTorque(const glm::vec3& additionalTorque) {
			appliedTorque += additionalTorque;
		}

		glm::vec3 GetAngularMomentum() const {
			return angularMomentum;
		}

		void SetAngularMomentum(const glm::vec3& angMom) {
			angularMomentum = angMom;
		}

		glm::mat3 GetInvInertiaTensor() const {
			glm::mat3 rotationMatrix = owner->GetRotationMatrix();
			return rotationMatrix * invModelSpaceMomentOfInertia * glm::transpose(rotationMatrix);
		}

		void SetMomentOfInertia(const glm::vec3& moi) {
			invModelSpaceMomentOfInertia = glm::mat3(
				1.0f / moi.x, 0.0f, 0.0f,
				0.0f, 1.0f / moi.y, 0.0f,
				0.0f, 0.0f, 1.0f / moi.z);
		}

		glm::vec3 GetRotationalDrag() const {
			return rotationalDrag;
		}

		void SetRotationalDrag(const glm::vec3& rDrag) {
			rotationalDrag = rDrag;
		}

		static glm::vec3 GetMomentOfInertiaOfCuboid(float mass, glm::vec3 scale) {
			return glm::vec3(
				mass / 12.0f * (4.0f * scale.y * scale.y + 4.0f * scale.z * scale.z),
				mass / 12.0f * (4.0f * scale.x * scale.x + 4.0f * scale.z * scale.z),
				mass / 12.0f * (4.0f * scale.x * scale.x + 4.0f * scale.y * scale.y)
			);
		}

		static glm::vec3 GetMomentOfInertiaOfSolidSphere(float mass, float radius) {
			return glm::vec3(
				mass * 2.0f / 5.0f * radius * radius,
				mass * 2.0f / 5.0f * radius * radius,
				mass * 2.0f / 5.0f * radius * radius
			);
		}

		static glm::vec3 GetMomentOfInertiaOfHollowSphere(float mass, float radius) {
			return glm::vec3(
				mass * 2.0f / 3.0f * radius * radius,
				mass * 2.0f / 3.0f * radius * radius,
				mass * 2.0f / 3.0f * radius * radius
			);
		}

		/*
		* The height is along y axis
		*/
		static glm::vec3 GetMomentOfInertiaOfSolidCylinder(float mass, float radius, float height) {
			return glm::vec3(
				mass / 12.0f * (3.0f * radius * radius + height * height),
				mass / 2.0f * radius * radius,
				mass / 12.0f * (3.0f * radius * radius + height * height)
			);
		}

		void Collide(Physics& b, const glm::vec3& wPoint, const glm::vec3& wNormal, float overlapAlongNormal);

		void ForcePositionOffset(const glm::vec3& offset) {
			forcedPositionOffsets.push_back(glm::vec3(offset));
		}

		void ForceOrientationOffset(const glm::quat& offset) {
			forcedOrientationOffsets.push_back(glm::quat(offset));
		}

		/*
		* [0..1] 0 - object can not be moved; 1 - object can be freely moved
		*/
		float GetPositionForcingAffinity() const {
			return positionForcingAffinity;
		}

		/*
		* [0..1] 0 - object can not be moved; 1 - object can be freely moved
		*/
		void SetPositionForcingAffinity(float level) {
			positionForcingAffinity = std::clamp(level, 0.0f, 1.0f);
		}

		float GetElasticity() const {
			return elasticity;
		}

		void SetElasticity(float e) {
			elasticity = std::clamp(e, 0.0f, 1.0f);
		}

		glm::vec3 GetAngularVelocity() {
			glm::mat3 rotationMatrix = owner->GetRotationMatrix();
			return rotationMatrix * invModelSpaceMomentOfInertia * glm::transpose(rotationMatrix) * angularMomentum;
		}

		float GetFriction() const {
			return friction;
		}

		void SetFriction(float f) {
			friction = std::clamp(f, 0.0f, 1.0f);
		}
	
		/*
		* Should be used only for collision between two Physics objects
		*/
		const glm::vec3& GetOwnerPosition() const {
			return owner->GetPosition();
		}

	private:
		SceneObject* owner;

		glm::vec3 appliedForce = glm::vec3(0.0f);
		glm::vec3 appliedTransientForce = glm::vec3(0.0f);
		glm::vec3 impulse = glm::vec3(0.0f);
		glm::vec3 momentum = glm::vec3(0.0f);						// HU: lendulet
		float invMass_1_per_kg = 0.0f;
		glm::vec3 modelSpaceDrag = glm::vec3(0.0f);
		glm::vec3 worldSpaceDrag = glm::vec3(0.0f);

		glm::vec3 appliedTorque = glm::vec3(0.0f);					// HU: forgato nyomatek
		glm::vec3 appliedTransientTorque = glm::vec3(0.0f);
		glm::vec3 impulseAsIntegratedTorque = glm::vec3(0.0f);
		glm::vec3 angularMomentum = glm::vec3(0.0f);				// HU: perdulet
		glm::mat3 invModelSpaceMomentOfInertia = glm::mat3(0.0f);		// HU: tehetetlensegi nyomatek inverze
		glm::vec3 rotationalDrag = glm::vec3(0.0f);
		float elasticity = 0.0f;	// From [0..1] interval: 1 - fully elastic; 0 - inelastic. 
		float friction = 0.5f;
		std::vector<glm::vec3> forcedPositionOffsets;
		std::vector<glm::quat> forcedOrientationOffsets;
		float positionForcingAffinity = 0; // [0..1] 0 - object can not be moved; 1 - object can be freely moved

	};

}