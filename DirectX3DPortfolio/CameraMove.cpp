#include "pch.h"
#include "CameraMove.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Transform.h"

void CameraMove::Update()
{
	float dt = TIME->GetDeltaTime();

	Vector3 pos = GetTransform()->GetPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * dt;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * dt;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * dt;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * dt;
	if (INPUT->GetButton(KEY_TYPE::Q))
		pos -= GetTransform()->GetUp() * _speed * dt;

	if (INPUT->GetButton(KEY_TYPE::E))
		pos += GetTransform()->GetUp() * _speed * dt;
	GetTransform()->SetPosition(pos);

	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		_lastMousePos = INPUT->GetMousePos();
		SetCapture(GAME->GetGameDesc().hWnd);
	}
	if(INPUT->GetButton(KEY_TYPE::RBUTTON))
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(INPUT->GetMousePos().x - _lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(INPUT->GetMousePos().y - _lastMousePos.y));

		Vector3 rot = GetTransform()->GetRotation();
		rot.y += dx;
		rot.x += dy;
		GetTransform()->SetRotation(rot);
		
		_lastMousePos.x = INPUT->GetMousePos().x;
		_lastMousePos.y = INPUT->GetMousePos().y;
	}
	if (INPUT->GetButtonUp(KEY_TYPE::RBUTTON))
	{
		ReleaseCapture();
	}


	

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		Vector3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += dt * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
	
		Vector3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= dt * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}
}
