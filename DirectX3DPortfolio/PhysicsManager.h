#pragma once

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

class GameObject;

class PhysicsManager
{
public:
	DECLARE_SINGLE(PhysicsManager);
	~PhysicsManager();
	void Init();
	void Release();
	void Update();

public:
	void AddActor(shared_ptr<GameObject>, PxActor* actor);

	PxPhysics* GetPhyisc() { return _physics; }
	PxScene* GetScene() { return _scene; }
	shared_ptr<GameObject> GetGameObjectMatchingRigidBody(PxActor* actor);
public:
	PxRigidDynamic* CreateBox(const Vector3& pos, const Vector3& size);
private:
	PxFoundation* _foundation;
	PxPvd* _pvd;
	PxPhysics* _physics;
	PxScene* _scene;
	PxMaterial* _material;
	PxDefaultCpuDispatcher* _dispatcher;
	PxPvdSceneClient* _pvdClient;
	PxArticulationReducedCoordinate* _articulation;


private:
    float _accumulator = 0.0f;
	float _stepSize = 1.0f / 60.0f;

	map<PxActor*, shared_ptr<GameObject>> _actorGameObjects;
};