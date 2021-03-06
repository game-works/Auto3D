#include "GeometryNode2D.h"
#include "../Debug/Log.h"
#include "../Graphics/Texture.h"

namespace Auto3D
{

GeometryNode2D::GeometryNode2D() :
	_geometryType(GeometryType::STATIC)
{
	SetFlag(NF_2D_GEOMETRY, true);
}

GeometryNode2D::~GeometryNode2D()
{
}

void GeometryNode2D::RegisterObject()
{
	RegisterFactory<GeometryNode2D>();
	CopyBaseAttributes<GeometryNode2D, SpatialNode2D>();
}

void GeometryNode2D::SetGeometryType(GeometryType::Type type)
{
	_geometryType = type;
}

void GeometryNode2D::SetGeometry(Geometry* geometry)
{
	if (!geometry)
	{
		ErrorString("Can not assign null geometry");
		return;
	}
	_geometry = geometry;

}

Geometry* GeometryNode2D::GetGeometry() const
{
	return _geometry;
}

void GeometryNode2D::SetTexture(Texture* texture)
{
	_texture = texture;

	SetGeometry(texture->GetGeometry());
}

}