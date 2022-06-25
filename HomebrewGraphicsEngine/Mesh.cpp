#include "Mesh.h"
namespace hograengine {

	Mesh::Mesh(Material* material, Geometry* geometry) : material(material), geometry(geometry)
	{
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
	}

	void Mesh::Draw() const
	{
		geometry->Draw();
	}

	void Mesh::DrawInstanced(const std::vector<Geometry::InstanceData>& data) const
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
}