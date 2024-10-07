#include "pch.h"
#include "Mesh.h"
#include "GeometryHelper.h"

Mesh::Mesh() : ResourceBase(ResourceType::Mesh)
{
}

Mesh::~Mesh()
{
}


void Mesh::CreateLine()
{
	_geometry = make_shared<Geometry<MeshVertexType>>();
	MeshVertexType v;
	v.position = Vector3::Zero;
	_geometry->AddVertex(v);
	v.position = Vector3::One;
	_geometry->AddVertex(v);
	_geometry->AddIndex(0);
	_geometry->AddIndex(1);
	CreateBuffers();
}

void Mesh::CreateQuad()
{
	_geometry = make_shared<Geometry<MeshVertexType>>();
	GeometryHelper::CreateQuad(_geometry);
	CreateBuffers();
}

void Mesh::CreateCube()
{
	_geometry = make_shared<Geometry<MeshVertexType>>();
	GeometryHelper::CreateCube(_geometry);
	CreateBuffers();
}

void Mesh::CreateGrid(int32 sizeX, int32 sizeZ)
{
	_geometry = make_shared<Geometry<MeshVertexType>>();
	GeometryHelper::CreateGrid(_geometry, sizeX, sizeZ);
	CreateBuffers();
}

void Mesh::CreateSphere()
{
	_geometry = make_shared<Geometry<MeshVertexType>>();
	GeometryHelper::CreateSphere(_geometry);
	CreateBuffers();
}
void Mesh::CreateBuffers()
{
	_vertexBuffer = make_shared<VertexBuffer>();
	if(_geometry)
		_vertexBuffer->Create<MeshVertexType>(_geometry->GetVertices());
	if (_modelGeometry)
		_vertexBuffer->Create<ModelVertexType>(_modelGeometry->GetVertices());

	_indexBuffer = make_shared<IndexBuffer>();
	if (_geometry)
		_indexBuffer->Create(_geometry->GetIndices());
	if (_modelGeometry)
		_indexBuffer->Create(_modelGeometry->GetIndices());
}


void Mesh::Load(const wstring& path)
{
}

void Mesh::Save(const wstring& path)
{
}

shared_ptr<Geometry<MeshVertexType>> Mesh::GetGeometry()
{
	if (_geometry == nullptr)
		_geometry = make_shared< Geometry<MeshVertexType>>();
	return _geometry;
}
shared_ptr<Geometry<ModelVertexType>> Mesh::GetModelGeometry()
{
	if (_modelGeometry == nullptr)
		_modelGeometry = make_shared< Geometry<ModelVertexType>>();
	return _modelGeometry;
}
