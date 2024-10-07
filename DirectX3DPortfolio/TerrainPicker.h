#pragma once
#include "MonoBehaviour.h"

class Terrain;
class Quadtree;

class TerrainPicker : public MonoBehaviour
{
public:
	TerrainPicker();
	~TerrainPicker();


public:
	void Pick(const Vector3& pos);
	void SetTerrain(shared_ptr<Terrain>& terrain);
	const Vector3& GetMousePositionOnTerrain(int32 sx, int32 sy);
private:
	Vector4 _rayOrigin;
	Vector4 _rayDir;

public:
	//temp
	Vector3 _point;
	vector<uint32> _indices;

	shared_ptr<Quadtree> _quadTree;
	shared_ptr<Terrain> _pickedTerrain;
	vector<int> _pickingIndices;
};

