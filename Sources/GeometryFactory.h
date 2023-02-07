#pragma once

#include "Geometry.h"
#include "MemoryManager.h"

namespace Hogra {

	/*
	* Factory that creates a singleton full screen quad geometry.
	*/
	class GeometryFactory
	{
		friend class Allocator;
	public:
		static GeometryFactory* GetInstance();
		static void DestroyInstance();

		Geometry* GetQuad();

		Geometry* GetSimpleQuad();

		Geometry* GetCube();

		Geometry* GetWireframeCube();

		Geometry* GetSphere();

		Geometry* GetWireFrameSphere();

		Geometry* GetCilinder();

		/*
		* The Front face is the opposite (uses counter-clockwise as front facing)
		*/
		Geometry* getLightVolumeSphere();

		void ForgetPointers();

	private:
		Geometry* GenerateSphere();
		void generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices);
		GeometryFactory() = default;
		static GeometryFactory* instance;

		Geometry* quad = nullptr;
		Geometry* simpleQuad = nullptr;
		Geometry* cube = nullptr;
		Geometry* wireframeCube = nullptr;
		Geometry* sphere = nullptr;
		Geometry* lightVolumeSphere = nullptr;
		Geometry* wireFrameSphere = nullptr;
		Geometry* cilinder = nullptr;
	};
}
