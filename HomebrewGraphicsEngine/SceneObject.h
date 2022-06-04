#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Light.h"
#include "Animation.h"
#include "FBO.h"


class SceneObject
{
public:
	SceneObject(Mesh* _mesh = nullptr) : mesh(_mesh) {}

	~SceneObject() = default;

	void update();

	virtual void control(float dt) {}

	virtual void animate(float dt);

	virtual void draw(const Camera& camera, const std::vector<Light*>& lights);

	void setMesh(Mesh* _mesh) {
		mesh = _mesh;
	}

	void setAnimation(Animation* _animation) {
		animation = _animation;
	}

	void setPosition(glm::vec3 pos) {
		position = pos;
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setScale(glm::vec3 s) {
		scale = s;
	}

	glm::vec3 getScale() {
		return scale;
	}

	void setEulerAngles(glm::vec3 angles) {
		eulerAngles = angles;
	}

	Mesh* getMesh() const {
		return mesh;
	}

protected:
	Mesh* mesh = nullptr;		// Don't delete from this object!

	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 eulerAngles = glm::vec3(0.0f);
	glm::vec3 position = glm::vec3(0.0f);

	Animation* animation = nullptr;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 invModelMatrix = glm::mat4(1.0f);

	/*
	* Exports model matrix and inverse model matrix into shader uniform
	*/
	void exportMatrices(const ShaderProgram& program);
};

