#include "Mesh.h"

Mesh::Mesh(Material* material, Geometry* geometry) : material(material), geometry(geometry)
{
}

void Mesh::Bind() const
{
	material->Bind();
}

void Mesh::Bind(const Camera& camera, const std::vector<Light*>& lights) const
{
	material->Bind(camera, lights);
}

void Mesh::Draw() const
{
	geometry->Draw();
}

Material* Mesh::getMaterial() const {
	return material;
}

Geometry* Mesh::getGeometry() const
{
	return geometry;
}
