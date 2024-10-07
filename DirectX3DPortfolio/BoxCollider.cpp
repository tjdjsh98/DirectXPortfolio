#include "pch.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "Utils.h"
#include "RigidBody.h"

BoxCollider::BoxCollider() : Component(ComponentType::BoxCollider)
{
}

BoxCollider::~BoxCollider()
{
	DisconnectRigidBody();
	_shape->release();
}

void BoxCollider::Init()
{
	auto physics = PHYSICS->GetPhyisc();
	auto scene = PHYSICS->GetScene();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.f);

	_scale = Vector3(1, 1, 1);
	_shape = physics->createShape(PxBoxGeometry(_scale.x / 2, _scale.y / 2, _scale.z / 2), *material);
	_shape->setLocalPose(PxTransform(Utils::ToPxVec3(_offset)));
	_shape->setContactOffset(0.2f);


	ConnectRigidBody();
}

void BoxCollider::Update()
{
	if (_connectedRigidBody == nullptr)
	{
		ConnectRigidBody();
	}

	PxVec3 rPos;
	if(_connectedRigidBody != nullptr)
		rPos = _connectedRigidBody->_rigidBody->getGlobalPose().p;
	else
		rPos = Utils::ToPxVec3(GetTransform()->GetPosition()+ _offset);

	GIZMO->DrawCube(Utils::ToVector3(rPos), GetScale(),Color(0,1,0,0));
}

void BoxCollider::SetScale(const Vector3 scale)
{
	_scale = scale;
	_shape->setGeometry(PxBoxGeometry(_scale.x / 2, _scale.y / 2, _scale.z / 2));
}

void BoxCollider::SetOffset(const Vector3 offset)
{
	_offset = offset;
	if (_shape != nullptr)
	{
		_shape->setLocalPose( PxTransform(Utils::ToPxVec3(_offset)));
	}
}

const Vector3& BoxCollider::GetScale()
{
	return _scale;
}

const Vector3& BoxCollider::GetOffset()
{
	return _offset;
}



void BoxCollider::ConnectRigidBody()
{
	shared_ptr<RigidBody> parentRigid;
	
	shared_ptr<Transform> tr = _gameObject.lock()->GetTransform();
	while (tr != nullptr && parentRigid == nullptr)
	{
		parentRigid = tr->GetGameObject()->GetCompoent<RigidBody>();
		tr = tr->GetParent();
	}
	_connectedRigidBody = parentRigid;

	if (_connectedRigidBody != nullptr) {
		_connectedRigidBody->_rigidBody->attachShape(*_shape);
	}
}

void BoxCollider::DisconnectRigidBody()
{
	if (_connectedRigidBody != nullptr)
	{
		_connectedRigidBody->_rigidBody->detachShape(*_shape);
	}
	_connectedRigidBody = nullptr;
}
