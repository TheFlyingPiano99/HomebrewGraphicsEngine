#include "Physics.h"
#include "HograTime.h"
#include <math.h>
#include <iostream>

namespace Hogra {

	void Physics::EarlyPhysicsUpdate(float dt)
	{
	}

	void Physics::LatePhysicsUpdate(float dt)
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
		momentum += (appliedForce + appliedTransientForce) * dt + impulse;
		glm::quat orientation = owner->GetOrientation();
		glm::vec3 rotatedModelSpaceDrag = abs(orientation * modelSpaceDrag);
		glm::vec3 drag = worldSpaceDrag + rotatedModelSpaceDrag;
		momentum *= glm::vec3(std::expf(-drag.x * dt * density), std::expf(-drag.y * dt * density), std::expf(-drag.z * dt * density));
		glm::vec3 pos = owner->GetPosition();
		pos += momentum * density * dt;
		owner->SetPosition(pos);

		//Rotation:
		angularMomentum += (appliedTorque + appliedTransientTorque) * dt + impulseAsIntegratedTorque;
		glm::mat3 rotationMatrix = owner->GetRotationMatrix();
		glm::vec3 modelSpaceRotationDrag = glm::vec3(
			std::expf(-rotationalDrag.x * dt * invModelSpaceInertiaTensor[0][0]),
			std::expf(-rotationalDrag.y * dt * invModelSpaceInertiaTensor[1][1]),
			std::expf(-rotationalDrag.z * dt * invModelSpaceInertiaTensor[2][2]));
		glm::mat3 msRotDragMat = glm::mat3(
			modelSpaceRotationDrag.x, 0.0f, 0.0f,
			0.0f, modelSpaceRotationDrag.y, 0.0f,
			0.0f, 0.0f, modelSpaceRotationDrag.z
		);
		angularMomentum = rotationMatrix * msRotDragMat * glm::transpose(rotationMatrix) * angularMomentum;	// with similarity transformation
		glm::vec3 rotation = dt * rotationMatrix * invModelSpaceInertiaTensor * glm::transpose(rotationMatrix) * angularMomentum; // with similarity transformation
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
		glm::vec3 ka = wPoint - this->getOwnerPosition();
		glm::vec3 kb = wPoint - b.getOwnerPosition();
		glm::vec3 va = this->getVelocity() + cross(this->getAngularVelocity(), ka);
		glm::vec3 vb = b.getVelocity() + cross(b.getAngularVelocity(), kb);
		float vRelNorm = glm::dot(wNormal, va - vb);
		float eb = b.getElasticity();
		float j = -(1.0f + elasticity * eb) * vRelNorm
			/ (this->getInvMass()
			+ b.getInvMass()
			+ glm::dot(wNormal, glm::cross(this->getInvInertiaTensor() * glm::cross(ka, wNormal), ka))
			+ glm::dot(wNormal, glm::cross(b.getInvInertiaTensor() * glm::cross(kb, wNormal), kb))
			);
		j = std::min(j, 0.0f);
		if (glm::vec3 tangV = va - vb - vRelNorm * wNormal; length(tangV) > 0.000000001f) {
			glm::vec3 tangent = glm::normalize(tangV);
			float vRelTangent = glm::dot(va - vb, tangent);
			float frictionJ = -friction * b.getFriction() * vRelTangent 
				/ (this->getInvMass()
				+ b.getInvMass()
				+ glm::dot(tangent, glm::cross(this->getInvInertiaTensor() * glm::cross(ka, tangent), ka))
				+ glm::dot(tangent, glm::cross(b.getInvInertiaTensor() * glm::cross(kb, tangent), kb))
				);
			this->ApplyImpulse(j * wNormal + frictionJ * tangent, ka);
			b.ApplyImpulse(j * -wNormal - frictionJ * tangent, kb);
		}
		else {	// Impulse without friction
			this->ApplyImpulse(j * wNormal, ka);
			b.ApplyImpulse(j * -wNormal, kb);
		}

		float pfla = positionForcingLevel;
		float pflb = b.getPositionForcingLevel();
		float sumPFL = pfla + pflb;
		if (sumPFL > 0.0f) {
			forcePositionOffset(pfla / sumPFL * overlapAlongNormal * -wNormal * 1.001f);
			b.forcePositionOffset(pflb / sumPFL * overlapAlongNormal * wNormal * 1.001f);
		}
	}
}