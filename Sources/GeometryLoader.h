#pragma once
#include "Geometry.h"
#include <string>

namespace Hogra {
	class GeometryLoader
	{
	public:

		class GeometryLoadingException : public std::exception {
		};

		Geometry* Load(const std::string& path);
	};
}

