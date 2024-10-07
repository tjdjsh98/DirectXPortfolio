#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "InstancingManager.h"
#include "Shader.h"
#include "Light.h"


Scene::Scene()
{
	_shader = RESOURCE->Get<Shader>(L"Default");
}

void Scene::Start()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->Start();
	}
}

void Scene::Update()
{
	/*clock_t start, finish;
	start = clock();*/

	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->Update();
	}

	PickUI();

	/*finish = clock();
	float duration = (double)(finish - start) / CLOCKS_PER_SEC;
	int a = 0;*/
}

void Scene::FixedUpdate()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->FixedUpdate();
	}
}

void Scene::LateUpdate()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->LateUpdate();
	}
	CheckCollision();
}

void Scene::Render()
{
	for (auto light : _lights)
	{
		light->PushData(_shader);
	}
	GlobalDesc globalDesc;
	{
		globalDesc.V = Camera::S_MatView;
		globalDesc.P = Camera::S_MatProjection;
		globalDesc.VP = globalDesc.V * globalDesc.P;
		globalDesc.VInv = globalDesc.V.Invert();
	}
	_shader->PushGlobalData(globalDesc);
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->Render();
	}
	for (auto& camera : _cameras)
	{
		camera->SortGameObjects();
		camera->Render_Forward();
	}

}

void Scene::Add(shared_ptr<GameObject> object)
{
	if (object == nullptr) return;

	_objects.insert(object);

	auto camera = object->GetCompoent<Camera>();
	if (camera != nullptr)
	{
		_cameras.insert(camera);
	}

	auto light = object->GetCompoent<Light>();
	if (light != nullptr)
	{
		_lights.insert(light);
	}

	object->Awake();
	object->Start();
}

void Scene::Add(const vector<shared_ptr<GameObject>>& objects)
{
	for (auto obj : objects)
	{
		Add(obj);
	}
}

void Scene::Remove(shared_ptr<GameObject> object)
{
	_objects.erase(object);

	auto camera = object->GetCompoent<Camera>();
	if (camera != nullptr)
	{
		_cameras.erase(camera);
	}

	auto light = object->GetCompoent<Light>();
	if (light != nullptr)
	{
		_lights.erase(light);
	}
}

shared_ptr<Camera> Scene::GetMainCamera()
{
	for (auto& camera : _cameras)
	{
		if (camera->GetProjectionType() == ProjectionType::Perspective)
			return camera;
	}

	return nullptr;
}

shared_ptr<Camera> Scene::GetUICamera()
{
	for (auto& camera : _cameras)
	{
		if (camera->GetProjectionType() == ProjectionType::Orthographic)
			return camera;
	}

	return nullptr;
}

void Scene::PickUI()
{
	/*if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) == false)
		return;

	if (GetUICamera() == nullptr)
		return;

	POINT screenPt = INPUT->GetMousePos();
	shared_ptr<Camera> camera = GetUICamera()->GetCamera();

	const auto gameObjects = GetObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetButton() == nullptr)
			continue;

		if (gameObject->GetButton()->Picked(screenPt))
		{
			gameObject->GetButton()->InvokeOnClicked();
		}
	}*/
}

shared_ptr<class GameObject> Scene::Pick(int32 screenX, int32 screenY)
{
	//shared_ptr<Camera> camera = GetMainCamera()->GetCamera();

	//float width = GRAPHICS->GetViewport().GetWidth();
	//float height = GRAPHICS->GetViewport().GetHeight();
	////float width = static_cast<float>(GAME->GetGameDesc().width);
	////float height = static_cast<float>(GAME->GetGameDesc().height);

	//Matrix projectionMatrix = camera->GetProjectionMatrix();

	//float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	//float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	//Matrix viewMatrix = camera->GetViewMatrix();
	//Matrix viewMatrixInv = viewMatrix.Invert();

	//const auto& gameObjects = GetObjects();

	//float minDistance = FLT_MAX;
	//shared_ptr<GameObject> picked;

	//for (auto& gameObject : gameObjects)
	//{
	//	if (camera->IsCulled(gameObject->GetLayerIndex()))
	//		continue;
	//	if (gameObject->GetCollider() == nullptr)
	//		continue;

	//	// ViewSpace에서의 Ray 정의
	//	Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//	Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

	//	// WorldSpace에서의 Ray 정의
	//	Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
	//	Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
	//	worldRayDir.Normalize();

	//	// WorldSpace에서 연산
	//	Ray ray = Ray(worldRayOrigin, worldRayDir);

	//	float distance = 0.f;
	//	if (gameObject->GetCollider()->Intersects(ray, OUT distance) == false)
	//		continue;

	//	if (distance < minDistance)
	//	{
	//		minDistance = distance;
	//		picked = gameObject;
	//	}
	//}

	//for (auto& gameObject : gameObjects)
	//{
	//	if (gameObject->GetTerrain() == nullptr)
	//		continue;

	//	Vec3 pickPos;
	//	float distance = 0.f;
	//	if (gameObject->GetTerrain()->Pick(screenX, screenY, OUT pickPos, OUT distance) == false)
	//		continue;

	//	if (distance < minDistance)
	//	{
	//		minDistance = distance;
	//		picked = gameObject;
	//	}
	//}

	//return picked;
	return nullptr;
}

void Scene::CheckCollision()
{
	//vector<shared_ptr<BaseCollider>> colliders;

	//for (shared_ptr<GameObject> object : _objects)
	//{
	//	if (object->GetCollider() == nullptr) continue;

	//	colliders.push_back(object->GetCollider());
	//}

	//// BruteForce
	//for (int32 i = 0; i < colliders.size(); i++)
	//{
	//	for (int j = i + 1; j < colliders.size(); j++)
	//	{
	//		shared_ptr<BaseCollider>& other = colliders[j];
	//		if (colliders[i]->Intersects(other))
	//		{
	//			int a = 3;
	//		}
	//	}
	//}
}
