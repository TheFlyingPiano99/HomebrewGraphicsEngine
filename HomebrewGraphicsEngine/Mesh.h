#pragma once

#include <string>
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowCaster.h"
#include "MemoryManager.h"

namespace Hogra {

	class Mesh
	{
		friend class Allocator;
	public:
		enum DepthTestFunc {
			less_func,
			greater_func
		};

		void Init(Material* material, Geometry* geometry);

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
		Material* material;
		Geometry* geometry;
		bool depthTest = true;
		bool stencilTest = false;
		DepthTestFunc depthTestFunc = DepthTestFunc::less_func;

		inline void* operator new(std::size_t size) { return ::operator new(size); }
	};
}