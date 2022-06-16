#pragma once

#include <string>
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Light.h"
namespace hograengine {

	class Mesh
	{

	public:

		// Initializes the mesh
		Mesh(Material* material, Geometry* geometry);

		// Draws the mesh
		void Bind() const;
		void Bind(const Camera& camera, const std::vector<Light*>& lights) const;
		void Draw() const;
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