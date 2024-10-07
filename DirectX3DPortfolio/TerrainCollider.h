#pragma once
#include "Component.h"

class Terrain;

class TerrainCollider: public Component
{
public:
	TerrainCollider();
	~TerrainCollider();

	virtual void Update() override;

	void CookingCollider();

	static ComponentType GetComponentType() { return ComponentType::TerrainCollider; }

	PxVec3 GetPosition()
	{
		return  _rigid->getGlobalPose().p;
	}

private:
	shared_ptr<Terrain> _connectedTerrain = nullptr;

private:
	PxMaterial* _material = nullptr;
	PxRigidStatic* _rigid = nullptr;
};

