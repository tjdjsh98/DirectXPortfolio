#pragma once
#include "Component.h"

class Mesh;
class Material;
class BoxCollider;

class RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();

	virtual void Init();
	virtual void Awake();
	virtual void Update();
	virtual void FixedUpdate();
 
	static ComponentType GetComponentType() { return ComponentType::RigidBody; }

	void SetVelocity(const Vector3& vel);


	void AddForce(const Vector3& force, PxForceMode::Enum mode = PxForceMode::eIMPULSE);
	void AddTorque(const Vector3& force, PxForceMode::Enum mode = PxForceMode::eIMPULSE);
	void SetLockAxis(const bool& x,const bool& y, const bool& z);

	void SetPosition(Vector3 pos);


private:
	Vector3 _offset;
private:
	friend BoxCollider;
	PxRigidDynamic* _rigidBody;
};

