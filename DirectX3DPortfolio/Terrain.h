#pragma once
#include "Component.h"

class Material;
class VertexBuffer;
class IndexBuffer;

struct TerrainDesc
{
	vector<shared_ptr<Material>> materials;
	string loadDataName;
	Vector2 size;
	float cellSpacing;
};

class Terrain : public Component
{
public:
	Terrain();
	~Terrain();

	static ComponentType GetComponentType() { return ComponentType::Terrain; }

	virtual void Update();
	virtual void Render();

	void SetPass(uint8 pass) { _pass = pass; }
	uint8 GetPass() { return _pass; }

	void ModifyHeight(uint32 index, int8 height,Vector3 point,float radius);
	void SmoothHeight(uint32 index,Vector3 point,float radius);
	void ModifyBlend(uint32 index, int8 blendIndex,Vector3 point,float radius);
	void CalcNormal();

private:
	void RenrerTesselation();
	void Smooth();
	float Average(int32 i, int32 j);
	bool InBounds(int32 i, int32 j);
	void CalcPatchBoundsY(uint32 i, uint32 j);

private:
	void BuildQuadPatchVB();
	void BuildQuadPatchIB();
	void BuildHeightSRV();
	void BuildBlendSRV();
public:
	void Create(const TerrainDesc& terrainDesc);
	void CreateRawVertices();
	void CreateRawIndices();
	void CreateTextureSRV();

	ComPtr<ID3D11ShaderResourceView> GetTextureSRV(int index);

	const vector<VertexTerrainData>& GetVertices() { return _vertices; }
	const vector<uint32>& GetIndices() { return _indices; }

	uint32 GetWidth() { return _terrainDesc.size.x; }
	uint32 GetDepth() { return _terrainDesc.size.y; }
	float GetCellSpacing() { return _terrainDesc.cellSpacing; }
	bool GetIsLockFrustums() { return _lockFrustums; }
	const vector<float>& GetHeightData() { return _heightMap; }
	vector<Vector3>* GetRawVerticesData() { return &_rawVertices; }
	const vector<Vector3>& GetRawVertices() { return _rawVertices; }
	const vector<uint32>& GetRawIndices() { return _rawIndices; }
	const TerrainDesc& GetTerrainDesc() { return _terrainDesc; }


	void SetShowBrush(bool enable) { _isShowBrush = enable; }
	void SetMousePos(Vector3 mousePos) { _mousePos = mousePos; }
	void SetBrushSize(float size) { _brushSize = size; }

public:
	void SaveMapData(string fileName);
	void LoadMapData(string fileName);


private:
	vector<Vector3> _rawVertices;
	vector<uint32> _rawIndices;


private:
	vector<VertexTerrainData> _vertices;
	ComPtr<ID3D11Buffer> _quadPatchVB;
	vector<uint32> _indices;
	ComPtr<ID3D11Buffer> _quadPatchIB;

	vector<float> _heightMap;
	ComPtr<ID3D11Texture2D> _heightMapTexture;
	ComPtr<ID3D11ShaderResourceView> _heightMapSrv;

	vector<Vector4> _blendMap;
	ComPtr<ID3D11Texture2D> _blendMapTexture;
	ComPtr<ID3D11ShaderResourceView> _blendMapSrv;


	ComPtr<ID3D11ShaderResourceView> _diffuseArraySrv;
	ComPtr<ID3D11ShaderResourceView> _normalArraySrv;
	ComPtr<ID3D11ShaderResourceView> _heightArraySrv;


private:
	bool _isShowBrush = false;
	Vector3 _mousePos;
	float _brushSize = 0;
	bool _heightChanged = false;
	bool _blendChanged = false;


	TerrainDesc _terrainDesc;
	shared_ptr<Material> _material;

	vector<Vector2> _patchBoundsY;

	uint8 _pass = 0;

	uint32 _numPatchVertexRows = 0;
	uint32 _numPatchVertexCols = 0;

	uint32 _numPatchVertices = 0;
	uint32 _numPatchQuadFaces = 0;

	const uint32 CellsPerPatch = 64;

	Vector4 _frustums[6];
	bool _lockFrustums;


};

