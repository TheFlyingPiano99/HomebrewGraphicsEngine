#pragma once

#include <string>
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "ShadowCaster.h"
#include "MemoryManager.h"
#include "Identifiable.h"

namespace Hogra {

	class Mesh : public Identifiable
	{
		ALLOCATOR_CONSTRUCTIBLE

	public:
		enum DepthTestFunc {
			less_func,
			greater_func
		};

		void Init(Material* material, Geometry* geometry);

		void Init(Geometry* geometry, Material* material);

		// Draws the mesh
		void Bind() const;
		void Draw() const;

		void DrawInstanced(const std::vector<Geometry::InstanceData>& data, int visibleCount) const;

		void DrawInstanced(const std::vector<Geometry::LightInstancedData>& data) const;

		Material* getMaterial() const;
		Geometry* getGeometry() const;

		void SetDepthTest(bool test);

		void setStencilTest(bool test);

		void setDepthFunc(const DepthTestFunc func);

	private:
		Material* volumeMaterial;
		Geometry* geometry;
		bool depthTest = true;
		bool stencilTest = false;
		DepthTestFunc depthTestFunc = DepthTestFunc::less_func;

	};
}