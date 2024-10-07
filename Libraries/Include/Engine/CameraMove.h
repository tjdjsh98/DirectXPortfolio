#pragma once
#include "MonoBehaviour.h"

class CameraMove : public MonoBehaviour
{
	virtual void Update();

private:
	float _speed = 10;
	POINT _lastMousePos = {};
};

