#include "pch.h"
#include "TerrainPicker.h"
#include "GameObject.h"
#include "Terrain.h"
#include "Camera.h"
#include "Quadtree.h"

TerrainPicker::TerrainPicker()
{
}

TerrainPicker::~TerrainPicker()
{
}



void TerrainPicker::Pick(const Vector3& pos)
{
	_pickingIndices.clear();
	if (_pickedTerrain == nullptr) return;


	for (int i = 0; i < _indices.size(); i++)
		_pickingIndices.push_back(_indices[i]);

	return;
}

void TerrainPicker::SetTerrain(shared_ptr<Terrain>& terrain)
{
	_pickedTerrain = terrain;
	
	_quadTree = make_shared<Quadtree>();
	_quadTree->Build(_pickedTerrain->GetRawVerticesData(), _pickedTerrain->GetRawIndices());
}

const Vector3& TerrainPicker::GetMousePositionOnTerrain(int32 sx, int32 sy)
{
	if (_pickedTerrain == nullptr) return Vector3::Zero;
	auto camera = CUR_SCENE->GetMainCamera();
	if (camera == nullptr) return Vector3::Zero;

	float centerX =GAME->GetGameDesc().width / 2;
	float centerY =GAME->GetGameDesc().height / 2;

	Vector3 cameraPos = camera->ScreenToWorldPosition(Vector4(centerX, centerY,0,1));
	Vector3 direction = camera->ScreenToWorldPosition(Vector4(sx, sy,1,0));
	direction.Normalize();

	_indices.clear();
	if (_quadTree->RayCastQuadIntersect(cameraPos, direction))
	{
		float distance = _quadTree->GetDistance();
		_point = cameraPos + direction * distance;
		for (auto idx : _quadTree->GetIntersectedIndices())
		{
			_indices.push_back(idx);
		}
	}
	return _point;
}
