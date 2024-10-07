//#pragma once
//
//class Terrain;
//class TerrainPicker;
//class Camera;
//
//enum class PaintType
//{
//	Height,
//	Smooth,
//	Texture,
//};
//
//class TerrainEditor : public IExcute
//{
//public:
//	TerrainEditor();
//	~TerrainEditor();
//
//	virtual void Init() override;
//	virtual void Update() override;
//	virtual void Render() override;
//
//	void Resize(uint32 sizeX, uint32 sizeZ);
//
//	void LoadHeightMapData(const wstring& path);
//	void SaveHeightMapData(const wstring& path);
//
//private:
//	void TestCastCast();
//
//	wstring colliderName= L"";
//	int size = 0;
//
//private:
//	shared_ptr<Shader> _shader;
//	shared_ptr<GameObject> _terrainObj;
//	shared_ptr<Terrain> _terrain;
//	shared_ptr<TerrainPicker> _terrainPicker;
//	shared_ptr<GameObject> _camera;
//	shared_ptr<GameObject> _obj;
//
//
//private:
//	vector<float> _heightData;
//	uint32 _sizeX;
//	uint32 _sizeZ;
//
//	Vector3 _resetPos{ 0,10,0 };
//
//	PaintType _paintType;
//	uint8 _paintIndex;
//
//	float _brushSize = 0.5f;
//	float _brushIntensity = 0.5f;
//	char _filename[128]{ 0, };
//};
//
