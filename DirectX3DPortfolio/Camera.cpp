#include "pch.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Terrain.h"
#include "SkinnedMeshRenderer.h"

Matrix Camera::S_MatView = Matrix::Identity;
Matrix Camera::S_MatProjection = Matrix::Identity;

Camera::Camera() : Component(ComponentType::Camera)
{
	_width = GAME->GetGameDesc().width;
	_height = GAME->GetGameDesc().height;
}

Camera::~Camera()
{
}



void Camera::Init()
{
	CUR_SCENE->AddCamera(shared_from_this());
}

void Camera::Update()
{
	Vector3 eyePosition = GetTransform()->GetPosition();
	Vector3 focusPosition = eyePosition + GetTransform()->GetLook();
	Vector3 upDirection = GetTransform()->GetUp();

	_matView = ::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	if (_type == ProjectionType::Perspective)
	{
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
	}
	else
	{
		_matProjection = ::XMMatrixOrthographicLH(_width, _height, _near, _far);
	}

	
}

void Camera::SortGameObjects()
{
	shared_ptr<Scene> curScene = CUR_SCENE;

	unordered_set<shared_ptr<GameObject>>& gameObjects = curScene->GetObjects();

	_vecForward.clear();

	for (auto gameObject : gameObjects)
	{
		if (gameObject->GetCompoent<MeshRenderer>() == nullptr
			&& gameObject->GetCompoent<SkinnedMeshRenderer>() == nullptr
			&& gameObject->GetCompoent<Terrain>() == nullptr)
			continue;

		_vecForward.push_back(gameObject);
	}
}

void Camera::Render_Forward()
{
	S_MatProjection = _matProjection;
	S_MatView = _matView;

	INSTANCING->Render(_vecForward);
}
Vector3 Camera::ScreenToWorldPosition(Vector4 screenPosition)
{
	
	float vx = (2.0f * screenPosition.x / GAME->GetGameDesc().width - 1.0f) / _matProjection(0, 0);
	float vy = (-2.0f * screenPosition.y / GAME->GetGameDesc().height + 1.0f) / _matProjection(1, 1);

	Matrix vInv = _matView.Invert();
	Vector3 position = Vector3::Zero;
	if(screenPosition.w == 1)
		position = XMVector3TransformCoord(Vector3(vx,vy,screenPosition.z), vInv);
	if (screenPosition.w == 0)
		position = XMVector3TransformNormal(Vector3(vx, vy, screenPosition.z), vInv);

	return position;
}