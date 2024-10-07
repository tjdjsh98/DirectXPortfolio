#include "pch.h"
#include "RigidBody.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include "Utils.h"

RigidBody::RigidBody() : Component(ComponentType::RigidBody)
{
  

}

RigidBody::~RigidBody()
{

}

void RigidBody::Init()
{
    auto physics = PHYSICS->GetPhyisc();
    auto scene = PHYSICS->GetScene();
    _rigidBody = physics->createRigidDynamic(PxTransform(Utils::ToPxVec3(_gameObject.lock()->GetTransform()->GetPosition())));
    PxRigidBodyExt::updateMassAndInertia(*_rigidBody, 0.5f);
    PHYSICS->AddActor(_gameObject.lock(), _rigidBody);


}

void RigidBody::Awake()
{
}

void RigidBody::Update()
{
    auto pxPos = _rigidBody->getGlobalPose().p;
    auto pxRot = _rigidBody->getGlobalPose().q;
    Quaternion quat(pxRot.x, pxRot.y, pxRot.z, pxRot.w);
    GetTransform()->SetPosition(Vector3(pxPos.x - _offset.x, pxPos.y - _offset.y, pxPos.z - _offset.z));
    GetTransform()->SetRotation(quat);
    
   
}

void RigidBody::FixedUpdate()
{
    PxVec3 angularVel = _rigidBody->getAngularVelocity();

    if (angularVel.x != 0)
        angularVel.x = 0;
    if (angularVel.z != 0)
        angularVel.z = 0;
    _rigidBody->setAngularVelocity(angularVel);

}


void RigidBody::SetVelocity(const Vector3& vel)
{
    _rigidBody->setLinearVelocity(Utils::ToPxVec3(vel));
}

void RigidBody::AddForce(const Vector3& force, PxForceMode::Enum mode)
{
    _rigidBody->addForce(Utils::ToPxVec3(force),PxForceMode::eIMPULSE);
}

void RigidBody::AddTorque(const Vector3& force, PxForceMode::Enum mode)
{
    _rigidBody->addTorque(Utils::ToPxVec3(force), PxForceMode::eIMPULSE);
}

void RigidBody::SetLockAxis(const bool& x, const bool& y, const bool& z)
{
    PxRigidDynamicLockFlags flags;
    if (x) flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
    if (y) flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
    if (z) flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
    _rigidBody->setRigidDynamicLockFlags(flags);
}

void RigidBody::SetPosition(Vector3 pos)
{
    _rigidBody->setGlobalPose(PxTransform(Utils::ToPxVec3(pos+_offset)));
}
