#pragma once

#include "Geometry.h"

namespace hograengine {
	namespace GeometryFactory {

		/*
		* Factory that creates a singleton full screen quad geometry.
		*/
		class FullScreenQuad
		{
		public:
			static Geometry* getInstance();
			static void destroyInstance();

		private:
			FullScreenQuad() = default;
			static Geometry* geometry;
			static Geometry* create();
		};

		/*
		* Factory that creates a singleton cube geometry.
		*/
		class Cube
		{
		public:
			static Geometry* getInstance();
			static void destroyInstance();

		private:
			Cube() = default;
			static Geometry* geometry;
			static Geometry* create();
		};

		class Sphere {
		public:
			static Geometry* getInstance();
			static void destroyInstance();

			private:
				static Geometry* create();

				static void generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLint>* indices);

				static Geometry* geometry;
		};
	}
}
