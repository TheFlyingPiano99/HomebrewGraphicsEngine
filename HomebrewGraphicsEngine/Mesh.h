#pragma once

#include <string>
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowCaster.h"

namespace hograengine {

	class Mesh
	{

	public:

		// Initializes the mesh
		Mesh(Material* material, Geometry* geometry);

		// Draws the mesh
		void Bind() const;
		void Draw() const;

		void DrawInstanced(const std::vector<Geometry::InstanceData>& data) const;

		Material* getMaterial() const;
		Geometry* getGeometry() const;

		void setDepthTest(bool test);
		void setStencilTest(bool test);

	private:
		Material* material;
		Geometry* geometry;
		bool depthTest = true;
		bool stencilTest = false;
	};
}