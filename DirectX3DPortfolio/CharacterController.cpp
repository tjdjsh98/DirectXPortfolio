#include "pch.h"
#include "CharacterController.h"
#include "RigidBody.h"
#include "Transform.h"

void CharacterController::Awake()
{
}

void CharacterController::Update()
{
	Move();
}

void CharacterController::Move()
{
	if (_rigidBody == nullptr) return;

	_rigidBody->SetVelocity(GetTransform()->GetLook() * _speed);
}
