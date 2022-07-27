#pragma once

#include <string>
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowCaster.h"

namespace Hogra {

	class Mesh
	{

	public:

		enum DepthTestFunc {
			less_func,
			greater_func
		};

		// Initializes the mesh
		Mesh(Material* material, Geometry* geometry);

		// Draws the mesh
		void Bind() const;
		void Draw() const;

		void DrawInstanced(const std::vector<Geometry::InstanceData>& data) const;

		void DrawInstanced(const std::vector<Geometry::LightInstancedData>& data) const;

		Material* getMaterial() const;
		Geometry* getGeometry() const;

		void setDepthTest(bool test);

		void setStencilTest(bool test);

		void setDepthFunc(const DepthTestFunc func);

	private:
		Material* material;
		Geometry* geometry;
		bool depthTest = true;
		bool stencilTest = false;
		DepthTestFunc depthTestFunc = DepthTestFunc::less_func;
	};
}