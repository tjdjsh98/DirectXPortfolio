#pragma once
#include "MonoBehaviour.h"

class RigidBody;

class CharacterController : public MonoBehaviour
{
public:
	virtual void Awake();
	virtual void Update();

private:
	void Move();


private:
	shared_ptr<RigidBody> _rigidBody;


	float _speed = 10;
};

