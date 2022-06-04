#pragma once

#include "Geometry.h"


namespace GeometryFactory {
	/*
	* Factory that creates a singleton full screen quad geometry.
	*/
	class FullScreenQuad
	{
	public:
		static Geometry* getInstance();

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

	private:
		Cube() = default;
		static Geometry* geometry;
		static Geometry* create();
	};
}

