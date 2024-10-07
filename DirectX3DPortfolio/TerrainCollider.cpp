#include "pch.h"
#include "TerrainCollider.h"
#include "Terrain.h"
#include "GameObject.h"

TerrainCollider::TerrainCollider() : Component(ComponentType::TerrainCollider)
{
    _material = PHYSICS->GetPhyisc()->createMaterial(0.5f, 0.5f, 0.f);
}

TerrainCollider::~TerrainCollider()
{
    _material->release();
}

void TerrainCollider::Update()
{
    if (_connectedTerrain == nullptr) {
        auto terrain = _gameObject.lock()->GetCompoent<Terrain>();
        if (terrain) 
        {
            _connectedTerrain = terrain;
            CookingCollider();
        }
    }
}

void TerrainCollider::CookingCollider()
{
  
    auto terrain = _gameObject.lock()->GetCompoent<Terrain>();
    if (terrain == nullptr) return;

    // 이미 만들어져 있다면 이전 형태는 모두 지워줍니다.
    if (_rigid != nullptr)
    {
        PxShape* shapes[10];
        ZeroMemory(shapes, sizeof(shapes));
        _rigid->getShapes(shapes, 10, 0);

        for (auto shape : shapes)
        {
            if (shape != nullptr) {
                _rigid->detachShape(*shape);
            }
        }
    }
    // 처음 만들어진 것이라면 강체를 추가합니다.
    else 
    {
        _rigid = PHYSICS->GetPhyisc()->createRigidStatic(PxTransform(PxIdentity));
        PHYSICS->AddActor(_gameObject.lock(), _rigid);
    }


    vector<Vector3>  vectices = terrain->GetRawVertices();

    const PxU32 ts = terrain->GetWidth()+1;


    PxReal minHeight = PX_MAX_F32;
    PxReal maxHeight = -PX_MAX_F32;

    for (PxU32 s = 0; s < ts * ts; s++)
    {
        minHeight = PxMin(minHeight, vectices[s].y);
        maxHeight = PxMax(maxHeight, vectices[s].y);
    }

    PxReal deltaHeight = maxHeight - minHeight;

    PxReal quantization = (PxReal)0x7fff;

    PxReal heightScale = PxMax(deltaHeight / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

    PxHeightFieldSample* hfSamples = new PxHeightFieldSample[ts * ts];

    PxU32 index = 0;
    for (PxU32 col = 0; col < ts; col++)
    {
        for (PxU32 row = 0; row < ts; row++)
        {
            PxI16 height;
            height = PxI16(quantization * ((vectices[(col*ts) + row].y - minHeight)) / deltaHeight);

            PxHeightFieldSample& smp = hfSamples[(row * ts) + col];
            smp.height = height;
            smp.materialIndex0 = 1;
            smp.materialIndex1 = 1;
            smp.setTessFlag();
        }
    }

    PxHeightFieldDesc terrainDesc;
    terrainDesc.format = PxHeightFieldFormat::eS16_TM;
    terrainDesc.nbColumns = ts;
    terrainDesc.nbRows = ts;
    terrainDesc.samples.data = hfSamples;
    terrainDesc.samples.stride = sizeof(PxHeightFieldSample);
    terrainDesc.flags = PxHeightFieldFlags();

    PxHeightFieldGeometry hfGeom;
    hfGeom.columnScale = terrain->GetCellSpacing();
    hfGeom.rowScale =  terrain->GetCellSpacing();
    hfGeom.heightScale = deltaHeight != 0.0f ? heightScale : 1.0f;
    hfGeom.heightField = PxCreateHeightField(terrainDesc, PHYSICS->GetPhyisc()->getPhysicsInsertionCallback());

    delete[] hfSamples;

    PxTransform localPos;
    float halfWidth = (int32)(terrain->GetWidth() * terrain->GetCellSpacing()) * 0.5f;
    float halfDepth = (int32)(terrain->GetDepth() * terrain->GetCellSpacing()) * 0.5f;

    localPos.p = PxVec3(-halfWidth, minHeight, -halfDepth);
    localPos.q = PxQuat(PxIdentity);
    PxShape* shape = PxRigidActorExt::createExclusiveShape(*_rigid, hfGeom, *_material);
    _rigid->setGlobalPose(localPos);
    shape->release();
    
}
