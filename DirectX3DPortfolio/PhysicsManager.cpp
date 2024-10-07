#include "pch.h"
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "Scene.h"

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

PhysicsManager::~PhysicsManager()
{
    Release();
}

void PhysicsManager::Init()
{
    _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!_foundation)
        assert(true);

    bool recordMemoryAllocations = true;

    _pvd = physx::PxCreatePvd(*_foundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    _pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);


    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation,
        physx::PxTolerancesScale(), recordMemoryAllocations, _pvd);
    if (!_physics)
        assert(true);

    PxSceneDesc sceneDesc(_physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

    PxU32 numCores = PxThread::getNbPhysicalCores();
    _dispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
    sceneDesc.cpuDispatcher = _dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.solverType = PxSolverType::eTGS;
    _material = _physics->createMaterial(0.5f, 0.5f, 0.f);


    _scene = _physics->createScene(sceneDesc);
    _pvdClient = _scene->getScenePvdClient();
    if (_pvdClient)
    {
        _pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        _pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        _pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    _articulation = _physics->createArticulationReducedCoordinate();
}
void PhysicsManager::Update()
{
    _accumulator += DT;
    if (_accumulator < _stepSize)
        return;

    int count = _accumulator / _stepSize;
    _accumulator -= _stepSize*count;

    CUR_SCENE->FixedUpdate();

    _scene->simulate(_stepSize*count);
    _scene->fetchResults(true);

    return;
}

void PhysicsManager::AddActor(shared_ptr<GameObject> go, PxActor* actor)
{
    _scene->addActor(*actor);

    if (_actorGameObjects.find(actor) == _actorGameObjects.end())
        assert(true);

    _actorGameObjects[actor] = go;
}

shared_ptr<GameObject> PhysicsManager::GetGameObjectMatchingRigidBody(PxActor* actor)
{
    shared_ptr<GameObject> go = _actorGameObjects[actor];

    return go;
}

PxRigidDynamic* PhysicsManager::CreateBox(const Vector3& pos, const Vector3& size)
{
    physx::PxRigidDynamic* box0 = _physics->createRigidDynamic(PxTransform(PxVec3(0.5f, 0.5f, 0.5f)));
    PxShape* shape0 = PxRigidActorExt::createExclusiveShape(*box0, PxBoxGeometry(0.5f), *_material);
    PxRigidBodyExt::updateMassAndInertia(*box0, 0.5f);
    _scene->addActor(*box0);

    const float contactOffset = 0.2f;
    shape0->setContactOffset(contactOffset);

    return box0;
}

void PhysicsManager::Release()
{
    _material->release();
    PX_RELEASE(_scene);
    
    PX_RELEASE(_physics);
    
    PX_RELEASE(_pvd);
    
    PxCloseExtensions();
    PX_RELEASE(_foundation);

    printf("SnippetArticulation done.\n");
}

