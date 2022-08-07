#include "Mesh.h"
#include "MemoryManager.h"
#include <iostream>

namespace Hogra {

	std::vector<Mesh*> Mesh::heapAllocatedInstances = std::vector<Mesh*>();

	Mesh* Mesh::Instantiate()
	{
		auto* instance = new Mesh();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}

	void Mesh::Init(Material* _material, Geometry* _geometry)
	{
		this->material = _material;
		this->geometry = _geometry;
	}

	void Mesh::Bind() const
	{
		material->Bind();
		if (depthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
		if (stencilTest) {
			glEnable(GL_STENCIL_TEST);
		}
		else {
			glDisable(GL_STENCIL_TEST);
		}
		switch (depthTestFunc)
		{
			case DepthTestFunc::less_func:
				glDepthFunc(GL_LESS);
				break;
			case DepthTestFunc::greater_func:
				glDepthFunc(GL_GREATER);
				break;
			default:
			break;
		}
	}

	void Mesh::Draw() const
	{
		geometry->Draw();
	}

	void Mesh::DrawInstanced(const std::vector<Geometry::InstanceData>& data, int visibleCount) const
	{
		geometry->DrawInstanced(data, visibleCount);
	}

	void Mesh::DrawInstanced(const std::vector<Geometry::LightInstancedData>& data) const
	{
		geometry->DrawInstanced(data);
	}

	Material* Mesh::getMaterial() const {
		return material;
	}

	Geometry* Mesh::getGeometry() const
	{
		return geometry;
	}

	void Mesh::setDepthTest(bool test)
	{
		depthTest = test;
	}

	void Mesh::setStencilTest(bool test)
	{
		stencilTest = test;
	}
	void Mesh::setDepthFunc(const DepthTestFunc func)
	{
		depthTestFunc = func;
	}
}