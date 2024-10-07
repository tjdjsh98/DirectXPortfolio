#pragma once
#include "ResourceBase.h"

class Model;
class Material;
struct ModelBone;

class Mesh : public ResourceBase
{
public:
	Mesh();
	~Mesh();

	void CreateLine();
	void CreateQuad();
	void CreateCube();
	void CreateGrid(int32 sizeX, int32 sizeZ);
	void CreateSphere();

	shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; }
	shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; }
	shared_ptr<Geometry<MeshVertexType>> GetGeometry();
	shared_ptr<Geometry<ModelVertexType>> GetModelGeometry();
	
	//shared_ptr<Model> GetModel() { return _model; }

	void SetBoneIndex(uint32 boneIndex) { _boneIndex = boneIndex; }
	uint32 GetBoneIndex() { return _boneIndex; }
	
	void SetModelMaterialName(wstring name) { _modelMaterialName = name; }
	wstring GetModelMaterialName() { return _modelMaterialName; }
	void CreateBuffers();

protected:
	virtual void Load(const wstring& path) override;
	virtual void Save(const wstring& path) override;

private:
	shared_ptr<Geometry<MeshVertexType>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;

	shared_ptr<Geometry<ModelVertexType>> _modelGeometry;

// Model Info
private:
	uint32 _boneIndex;
	wstring _modelMaterialName;
	shared_ptr<class Material> _modelCacheMaterial;
	weak_ptr<Model> _model;

public:
	shared_ptr<Material> material;
	shared_ptr<ModelBone> bone;
};

