#pragma once

#include "Geometry.h"

namespace hograengine {

	/*
	* Factory that creates a singleton full screen quad geometry.
	*/
	class GeometryFactory
	{
	public:
		static GeometryFactory* getInstance();
		static void destroyInstance();

		Geometry* getFullScreenQuad();

		Geometry* getCube();

		Geometry* getWireframeCube();

		Geometry* getSphere();

	private:
		static void generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLint>* indices);
		GeometryFactory() = default;
		static GeometryFactory* instance;

		Geometry* fullScreenQuad = nullptr;
		Geometry* cube = nullptr;
		Geometry* wireframeCube = nullptr;
		Geometry* sphere = nullptr;
	};
}
