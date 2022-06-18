#include "Physics.h"
#include <math.h>
#include <iostream>
namespace hograengine {

	void Physics::control(float dtSec)
	{

	}

	void Physics::update(float dtSec)
	{
		if (!forcedPositionOffsets.empty()) {
			glm::vec3 sumOffset = glm::vec3(0.0f);	// Position offset constraints
			float n = (float)forcedPositionOffsets.size();
			for (auto& offset : forcedPositionOffsets) {
				sumOffset += *offset;
				delete offset;
			}
			forcedPositionOffsets.clear();
			owner->setPosition(owner->getPosition() + sumOffset / n);
		}
		if (!forcedOrientationOffsets.empty()) {
			glm::quat cumOffset = angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));	// Orientation offset constraints
			float n = (float)forcedOrientationOffsets.size();
			for (auto& offset : forcedOrientationOffsets) {
				cumOffset *= *offset;
				delete offset;
			}
			forcedOrientationOffsets.clear();
			owner->setOrientation(cumOffset * owner->getOrientation());
		}
		//Movement:
		momentum += (appliedForce + appliedTransientForce) * dtSec + impulse;
		glm::quat orientation = owner->getOrientation();
		glm::vec3 rotatedModelSpaceDrag = abs(orientation * modelSpaceDrag);
		glm::vec3 drag = worldSpaceDrag + rotatedModelSpaceDrag;
		momentum *= glm::vec3(std::expf(-drag.x * dtSec * invMass), std::expf(-drag.y * dtSec * invMass), std::expf(-drag.z * dtSec * invMass));
		glm::vec3 pos = owner->getPosition();
		pos += momentum * invMass * dtSec;
		owner->setPosition(pos);

		//Rotation:
		angularMomentum += (appliedTorque + appliedTransientTorque) * dtSec + impulseAsIntegratedTorque;
		glm::mat3 rotationMatrix = owner->getRotationMatrix();
		glm::vec3 modelSpaceRotationDrag = glm::vec3(
			std::expf(-rotationalDrag.x * dtSec * invModelSpaceInertiaTensor[0][0]),
			std::expf(-rotationalDrag.y * dtSec * invModelSpaceInertiaTensor[1][1]),
			std::expf(-rotationalDrag.z * dtSec * invModelSpaceInertiaTensor[2][2]));
		glm::mat3 msRotDragMat = glm::mat3(
			modelSpaceRotationDrag.x, 0.0f, 0.0f,
			0.0f, modelSpaceRotationDrag.y, 0.0f,
			0.0f, 0.0f, modelSpaceRotationDrag.z
		);
		angularMomentum = rotationMatrix * msRotDragMat * glm::transpose(rotationMatrix) * angularMomentum;
		glm::vec3 rotation = dtSec * rotationMatrix * invModelSpaceInertiaTensor * glm::transpose(rotationMatrix) * angularMomentum;
		if (float ang = length(rotation); ang > 0.0f) {
			orientation = glm::angleAxis(ang, glm::normalize(rotation)) * orientation;
			owner->setOrientation(orientation);
		}

		// Clear transient:
		appliedTransientForce = glm::vec3(0.0f, 0.0f, 0.0f);
		appliedTransientTorque = glm::vec3(0.0f, 0.0f, 0.0f);
		impulse = glm::vec3(0.0f, 0.0f, 0.0f);
		impulseAsIntegratedTorque = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void Physics::collide(Physics& b, const glm::vec3& point, const glm::vec3& normal, float overlapAlongNormal) {
		glm::vec3 va = this->getVelocity();
		glm::vec3 vb = b.getVelocity();
		float vRel = glm::dot(normal, va - vb);
		glm::vec3 ka = point - this->getOwnerPosition();
		glm::vec3 kb = point - b.getOwnerPosition();
		float j = -(1.0f + elasticity * b.getElasticity()) * vRel / (this->getInvMass()
			+ b.getInvMass()
			+ glm::dot(normal, this->getInvInertiaTensor() * glm::cross(glm::cross(ka, normal), ka))
			+ glm::dot(normal, b.getInvInertiaTensor() * glm::cross(glm::cross(kb, normal), kb))
			);
		j = std::min(j, 0.0f);
		this->applyImpulse(j * normal, ka);
		b.applyImpulse(j * -normal, kb);

		float pfl0 = positionForcingLevel;
		float pfl1 = b.getPositionForcingLevel();
		float sumPFL = pfl0 + pfl1;
		if (sumPFL > 0.0f) {
			forcePositionOffset(pfl0 / sumPFL * overlapAlongNormal * -normal * 1.001f);
			b.forcePositionOffset(pfl1 / sumPFL * overlapAlongNormal * normal * 1.001f);
		}
	}
}