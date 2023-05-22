#include "Physics.h"
#include "HograTime.h"
#include <math.h>
#include <iostream>

namespace Hogra {

	void Physics::EarlyPhysicsUpdate(float dt_sec)
	{
	}

	void Physics::LatePhysicsUpdate(float dt_sec)
	{
		if (!forcedPositionOffsets.empty()) {
			glm::vec3 sumOffset = glm::vec3(0.0f);	// Position offset constraints
			float n = (float)forcedPositionOffsets.size();
			for (auto& offset : forcedPositionOffsets) {
				sumOffset += offset;
			}
			forcedPositionOffsets.clear();
			owner->SetPosition(owner->GetPosition() + sumOffset / n);
		}
		if (!forcedOrientationOffsets.empty()) {
			glm::quat cumOffset = angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));	// Orientation offset constraints
			float n = (float)forcedOrientationOffsets.size();
			for (auto& offset : forcedOrientationOffsets) {
				cumOffset *= offset;
			}
			forcedOrientationOffsets.clear();
			owner->SetOrientation(cumOffset * owner->GetOrientation());
		}
		//Movement:
		momentum += (appliedForce + appliedTransientForce) * dt_sec + impulse;
		glm::quat orientation = owner->GetOrientation();
		glm::vec3 rotatedModelSpaceDrag = abs(orientation * modelSpaceDrag);
		glm::vec3 drag = worldSpaceDrag + rotatedModelSpaceDrag;
		momentum *= glm::vec3(
			std::expf(-dt_sec * drag.x * invMass_1_per_kg),
			std::expf(-dt_sec * drag.y * invMass_1_per_kg),
			std::expf(-dt_sec * drag.z * invMass_1_per_kg)
		);
		glm::vec3 pos = owner->GetPosition();
		pos += momentum * invMass_1_per_kg * dt_sec;
		owner->SetPosition(pos);

		//Rotation:
		angularMomentum += (appliedTorque + appliedTransientTorque) * dt_sec + impulseAsIntegratedTorque;
		glm::mat3 rotationMatrix = owner->GetRotationMatrix();
		glm::vec3 modelSpaceRotationDrag = glm::vec3(
			std::expf(-rotationalDrag.x * dt_sec * invModelSpaceMomentOfInertia[0][0]),
			std::expf(-rotationalDrag.y * dt_sec * invModelSpaceMomentOfInertia[1][1]),
			std::expf(-rotationalDrag.z * dt_sec * invModelSpaceMomentOfInertia[2][2]));
		glm::mat3 msRotDragMat = glm::mat3(
			modelSpaceRotationDrag.x, 0.0f, 0.0f,
			0.0f, modelSpaceRotationDrag.y, 0.0f,
			0.0f, 0.0f, modelSpaceRotationDrag.z
		);
		angularMomentum = rotationMatrix * msRotDragMat * glm::transpose(rotationMatrix) * angularMomentum;	// with similarity transformation
		glm::vec3 rotation = dt_sec * rotationMatrix * invModelSpaceMomentOfInertia * glm::transpose(rotationMatrix) * angularMomentum; // with similarity transformation
		if (float ang = length(rotation); ang > 0.0f) {
			orientation = glm::angleAxis(ang, glm::normalize(rotation)) * orientation;
			owner->SetOrientation(orientation);
		}

		// Clear transient:
		appliedTransientForce = glm::vec3(0.0f, 0.0f, 0.0f);
		appliedTransientTorque = glm::vec3(0.0f, 0.0f, 0.0f);
		impulse = glm::vec3(0.0f, 0.0f, 0.0f);
		impulseAsIntegratedTorque = glm::vec3(0.0f, 0.0f, 0.0f);
	}


	void Physics::Collide(Physics& b, const glm::vec3& wPoint, const glm::vec3& wNormal, float overlapAlongNormal) {
		glm::vec3 ka = wPoint - this->GetOwnerPosition();
		glm::vec3 kb = wPoint - b.GetOwnerPosition();
		glm::vec3 va = this->GetVelocity() + cross(this->GetAngularVelocity(), ka);
		glm::vec3 vb = b.GetVelocity() + cross(b.GetAngularVelocity(), kb);
		float vRelNorm = glm::dot(wNormal, va - vb);
		float eb = b.GetElasticity();
		float j = -(1.0f + elasticity * eb) * vRelNorm
			/ (this->GetInvMass()
			+ b.GetInvMass()
			+ glm::dot(wNormal, glm::cross(this->GetInvInertiaTensor() * glm::cross(ka, wNormal), ka))
			+ glm::dot(wNormal, glm::cross(b.GetInvInertiaTensor() * glm::cross(kb, wNormal), kb))
			);
		j = std::min(j, 0.0f);
		if (glm::vec3 tangV = va - vb - vRelNorm * wNormal; length(tangV) > 0.000000001f) {
			glm::vec3 tangent = glm::normalize(tangV);
			float vRelTangent = glm::dot(va - vb, tangent);
			float frictionJ = -friction * b.GetFriction() * vRelTangent 
				/ (this->GetInvMass()
				+ b.GetInvMass()
				+ glm::dot(tangent, glm::cross(this->GetInvInertiaTensor() * glm::cross(ka, tangent), ka))
				+ glm::dot(tangent, glm::cross(b.GetInvInertiaTensor() * glm::cross(kb, tangent), kb))
				);
			this->ApplyImpulse(j * wNormal + frictionJ * tangent, ka);
			b.ApplyImpulse(j * -wNormal - frictionJ * tangent, kb);
		}
		else {	// Impulse without friction
			this->ApplyImpulse(j * wNormal, ka);
			b.ApplyImpulse(j * -wNormal, kb);
		}

		float pfla = positionForcingAffinity;
		float pflb = b.GetPositionForcingAffinity();
		float sumPFL = pfla + pflb;
		if (sumPFL > 0.0f) {
			ForcePositionOffset(pfla / sumPFL * overlapAlongNormal * -wNormal * 1.001f);
			b.ForcePositionOffset(pflb / sumPFL * overlapAlongNormal * wNormal * 1.001f);
		}
	}
}