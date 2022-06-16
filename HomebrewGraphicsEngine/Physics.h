#pragma once
#include "Component.h"
#include<glm/glm.hpp>
#include "SceneObject.h"
namespace hograengine {

	class Physics : public Component
	{
	public:
		explicit Physics(SceneObject* _owner) : Component(), owner(_owner) {}
		~Physics() override = default;

		// Inherited via Component
		void control(float dt) override;
		void update(float dtMil) override;

		/*
		* return force applied to center of mass in Newtons
		*/
		glm::vec3 getAppliedForce() const {
			return appliedForce;
		}

		/*
		* set force applied to center of mass in Newtons
		*/
		void setAppliedForce(const glm::vec3& force) {
			appliedForce = force;
		}

		/*
		* add force applied to center of mass in Newtons
		*/
		void addAppliedForce(const glm::vec3& additionalForce) {
			appliedForce += additionalForce;
		}

		void applyImpulse(const glm::vec3& _impulse, const glm::vec3& momentArm) {
			impulse += _impulse;
			impulseAsIntegratedTorque += glm::cross(momentArm, _impulse);
		}

		void applyTransientForce(const glm::vec3& force) {
			appliedTransientForce += force;
		}

		/*
		* get momentum [kg * m / s]
		*/
		glm::vec3 getMomentum() const {
			return momentum;
		}

		/*
		* set momentum [kg * m / s]
		*/
		void setMomentum(const glm::vec3& p) {
			momentum = p;
		}

		/*
		* get velocity in m/s
		*/
		glm::vec3 getVelocity() const {
			return momentum * invMass;
		}

		/*
		* set velocity in m/s
		*/
		void setVelocity(const glm::vec3& v) {
			momentum = v / invMass;
		}

		/*
		* get mass in kg
		*/
		float getMass() const {
			return 1.0f / invMass;
		}

		/*
		* get inverse of mass in 1/kg
		*/
		float getInvMass() const {
			return invMass;
		}

		/*
		* set mass in kg
		*/
		void setMass(const float m) {
			invMass = 1.0f / m;
		}

		glm::vec3 getModelSpaceDrag() const {
			return modelSpaceDrag;
		}

		void setModelSpaceDrag(const glm::vec3& mDrag) {
			modelSpaceDrag = mDrag;
		}

		glm::vec3 getWorldSpaceDrag() const {
			return worldSpaceDrag;
		}

		void setWorldSpaceDrag(const glm::vec3& wDrag) {
			worldSpaceDrag = wDrag;
		}

		glm::vec3 getAppliedTorque() const {
			return appliedTorque;
		}

		void setAppliedTorque(const glm::vec3& torque) {
			appliedTorque = torque;
		}

		void addAppliedTorque(const glm::vec3& additionalTorque) {
			appliedTorque += additionalTorque;
		}

		glm::vec3 getAngularMomentum() const {
			return angularMomentum;
		}

		void setAngularMomentum(const glm::vec3& angMom) {
			angularMomentum = angMom;
		}

		glm::mat3 getInvInertiaTensor() const {
			return invModelSpaceInertiaTensor;
		}

		void setMomentOfInertia(const glm::vec3& moi) {
			invModelSpaceInertiaTensor = glm::mat3(
				1.0f / moi.x, 0.0f, 0.0f,
				0.0f, 1.0f / moi.y, 0.0f,
				0.0f, 0.0f, 1.0f / moi.z);
		}

		glm::vec3 getRotationalDrag() const {
			return rotationalDrag;
		}

		void setRotationalDrag(const glm::vec3& rDrag) {
			rotationalDrag = rDrag;
		}

		const glm::vec3& getOwnerPosition() const {
			return owner->getPosition();
		}

		glm::vec3 getOwnerScale() const {
			return owner->getScale();
		}

		glm::quat getOwnerOrientation() const {
			return owner->getOrientation();
		}

		static glm::vec3 getMomentOfInertiaOfCuboid(float mass, glm::vec3 scale) {
			return glm::vec3(
				mass / 12.0f * (scale.y * scale.y + scale.z * scale.z),
				mass / 12.0f * (scale.x * scale.x + scale.z * scale.z),
				mass / 12.0f * (scale.x * scale.x + scale.y * scale.y)
			);
		}

		static glm::vec3 getMomentOfInertiaOfSolidSphere(float mass, float radius) {
			return glm::vec3(
				mass * 2.0f / 5.0f * radius * radius,
				mass * 2.0f / 5.0f * radius * radius,
				mass * 2.0f / 5.0f * radius * radius
			);
		}

		static glm::vec3 getMomentOfInertiaOfHollowSphere(float mass, float radius) {
			return glm::vec3(
				mass * 2.0f / 3.0f * radius * radius,
				mass * 2.0f / 3.0f * radius * radius,
				mass * 2.0f / 3.0f * radius * radius
			);
		}

		/*
		* The height is along y axis
		*/
		static glm::vec3 getMomentOfInertiaOfSolidCylinder(float mass, float radius, float height) {
			return glm::vec3(
				mass / 12.0f * (3.0f * radius * radius + height * height),
				mass / 2.0f * radius * radius,
				mass / 12.0f * (3.0f * radius * radius + height * height)
			);
		}

		void collide(Physics& b, const glm::vec3& point, const glm::vec3& normal, float overlapAlongNormal, float elasticity);

		void forcePositionOffset(const glm::vec3& offset) {
			forcedPositionOffsets.push_back(new glm::vec3(offset));
		}

		/*
		* [0..1] 0 ~ object can not be moved; 1 ~ object can be freely moved
		*/
		float getPositionForcingLevel() const {
			return positionForcingLevel;
		}

		/*
		* [0..1] 0 ~ object can not be moved; 1 ~ object can be freely moved
		*/
		void setPositionForcingLevel(float level) {
			positionForcingLevel = level;
		}

	private:
		SceneObject* owner;

		glm::vec3 appliedForce = glm::vec3(0.0f);
		glm::vec3 appliedTransientForce = glm::vec3(0.0f);
		glm::vec3 impulse = glm::vec3(0.0f);
		glm::vec3 momentum = glm::vec3(0.0f);
		float invMass = 0.0f;
		glm::vec3 modelSpaceDrag = glm::vec3(0.0f);
		glm::vec3 worldSpaceDrag = glm::vec3(0.0f);

		glm::vec3 appliedTorque = glm::vec3(0.0f);
		glm::vec3 impulseAsIntegratedTorque = glm::vec3(0.0f);
		glm::vec3 angularMomentum = glm::vec3(0.0f);
		glm::mat3 invModelSpaceInertiaTensor = glm::mat3(0.0f);
		glm::vec3 rotationalDrag = glm::vec3(0.0f);

		std::vector<glm::vec3*> forcedPositionOffsets;
		float positionForcingLevel = 0; // [0..1] 0 ~ object can not be moved; 1 ~ object can be freely moved
	};

}