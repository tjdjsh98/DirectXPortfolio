#pragma once
#include "Component.h"

class RigidBody;

class BoxCollider : public Component
{
public:
	BoxCollider();
	~BoxCollider();

	virtual void Init();
	virtual void Update();
	static ComponentType GetComponentType() { return ComponentType::BoxCollider; }

	const Vector3& GetScale();
	const Vector3& GetOffset();

	void SetScale(const Vector3 scale);
	void SetOffset(const Vector3 offset);


private:
	void ConnectRigidBody();
	void DisconnectRigidBody();

private:
	shared_ptr<RigidBody> _connectedRigidBody;

	PxShape* _shape;
	Vector3 _scale;
	Vector3 _offset;
};

