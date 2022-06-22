#pragma once
#include "Geometry.h"
#include <string>

namespace hograengine {
	class GeometryLoader
	{
	public:

		class GeometryLoadingException : public std::exception {
		};

		Geometry* load(const std::string& path);
	};
}

