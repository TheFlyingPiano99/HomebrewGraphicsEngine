#include "Physics.h"
#include <math.h>
#include <iostream>

void Physics::control(float dt)
{

}

void Physics::update(float dtMil)
{
	float dtSec = dtMil * 0.001f;
	//Movement:
	momentum += appliedForce * dtSec + impulse;
	glm::quat orientation = owner->getOrientation();
	glm::vec3 rotatedModelSpaceDrag = abs(orientation * modelSpaceDrag);
	glm::vec3 drag = worldSpaceDrag + rotatedModelSpaceDrag;
	momentum *= glm::vec3(std::expf(-drag.x * dtSec * invMass), std::expf(-drag.y * dtSec * invMass), std::expf(-drag.z * dtSec * invMass));
	glm::vec3 pos = owner->getPosition();
	pos += momentum * invMass * dtSec;
	owner->setPosition(pos);

	//Rotation:
 	angularMomentum += appliedTorque * dtSec + impulseAsIntegratedTorque;
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

	// Clear impulse:
	impulse = glm::vec3(0.0f);
	impulseAsIntegratedTorque = glm::vec3(0.0f);
}
