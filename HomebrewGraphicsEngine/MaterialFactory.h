#pragma once
#include "Material.h"
#include "TextureCube.h"

namespace hograengine {
	class MaterialFactory
	{
	public:
		Material* createPBR(const char* materialName, const Texture* skyBox);
	};
}
