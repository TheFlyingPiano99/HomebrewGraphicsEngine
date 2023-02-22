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
		angularMomentum = rotationMatrix * msRotDragMat * glm::transpose(rotationMatrix) * angularMomentum;
		glm::vec3 rotation = dt * rotationMatrix * invModelSpaceInertiaTensor * glm::transpose(rotationMatrix) * angularMomentum;
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


	void Physics::Collide(Physics& b, const glm::vec3& point, const glm::vec3& normal, float overlapAlongNormal) {
		glm::vec3 ka = point - this->getOwnerPosition();
		glm::vec3 kb = point - b.getOwnerPosition();
		glm::vec3 va = this->getVelocity() + cross(this->getAngularVelocity(), ka);
		glm::vec3 vb = b.getVelocity() + cross(b.getAngularVelocity(), kb);
		float vRelNorm = glm::dot(normal, va - vb);
		float eb = b.getElasticity();
		float j = -(1.0f + elasticity * eb) * vRelNorm
			/ (this->getInvMass()
			+ b.getInvMass()
			+ glm::dot(normal, glm::cross(this->getInvInertiaTensor() * glm::cross(ka, normal), ka))
			+ glm::dot(normal, glm::cross(b.getInvInertiaTensor() * glm::cross(kb, normal), kb))
			);
		j = std::min(j, 0.0f);
		if (glm::vec3 tangV = va - vb - vRelNorm * normal; length(tangV) > 0.000000001f) {
			glm::vec3 tangent = glm::normalize(tangV);
			float vRelTangent = glm::dot(va - vb, tangent);
			float frictionJ = -friction * b.getFriction() * vRelTangent 
				/ (this->getInvMass()
				+ b.getInvMass()
				+ glm::dot(tangent, glm::cross(this->getInvInertiaTensor() * glm::cross(ka, tangent), ka))
				+ glm::dot(tangent, glm::cross(b.getInvInertiaTensor() * glm::cross(kb, tangent), kb))
				);
			this->ApplyImpulse(j * normal + frictionJ * tangent, ka);
			b.ApplyImpulse(j * -normal - frictionJ * tangent, kb);
		}
		else {	// Impulse without friction
			this->ApplyImpulse(j * normal, ka);
			b.ApplyImpulse(j * -normal, kb);
		}

		float pfl0 = positionForcingLevel;
		float pfl1 = b.getPositionForcingLevel();
		float sumPFL = pfl0 + pfl1;
		if (sumPFL > 0.0f) {
			forcePositionOffset(pfl0 / sumPFL * overlapAlongNormal * -normal * 1.001f);
			b.forcePositionOffset(pfl1 / sumPFL * overlapAlongNormal * normal * 1.001f);
		}
	}
}