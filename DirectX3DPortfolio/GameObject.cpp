#include "pch.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "TerrainCollider.h"
#include "SkyBox.h"
#include "Animator.h"
#include "Light.h"

GameObject::GameObject(const wstring& name)
{
	_name = name;
}

GameObject::~GameObject()
{
}

void GameObject::Awake()
{
	for (auto component : _components)
	{
		if(component)
			component->Awake();
	}

	for (auto script : _scripts)
	{
		script->Awake();
	}

}

void GameObject::Start()
{
	for (auto component : _components)
	{
		if (component)
			component->Start();
	}

	for (auto script : _scripts)
	{
		script->Start();
	}
}

void GameObject::Update()
{
	for (auto component : _components)
	{
		if (component)
			component->Update();
	}

	for (auto script : _scripts)
	{
		script->Update();
	}
}

void GameObject::LateUpdate()
{
	for (auto component : _components)
	{
		if (component)
			component->LateUpdate();
	}

	for (auto script : _scripts)
	{
		script->LateUpdate();
	}
}

void GameObject::FixedUpdate()
{
	for (auto component : _components)
	{
		if (component)
			component->FixedUpdate();
	}

	for (auto script : _scripts)
	{
		script->FixedUpdate();
	}
}

void GameObject::Render()
{
	for (auto component : _components)
	{
		if (component)
			component->Render();
	}

}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());

	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;

		if (component->GetType() == ComponentType::Camera)
		{
			CUR_SCENE->AddCamera(static_pointer_cast<Camera>(component));
		}
	}
	else 
	{
		_scripts.push_back(static_pointer_cast<MonoBehaviour>(component));
	}

	component->Init();

}

void GameObject::AddComponent(ComponentType type)
{
	switch (type)
	{
	case ComponentType::Transform:
		AddComponent<Transform>();
		break;
	case ComponentType::MeshRenderer:
		AddComponent<MeshRenderer>();
		break;
	case ComponentType::Camera:
		AddComponent<Camera>();
		break;
	case ComponentType::SkinnedMeshRenderer:
		AddComponent<SkinnedMeshRenderer>();
		break;
	case ComponentType::Animator:
		AddComponent<Animator>();
		break;
	case ComponentType::Terrain:
		AddComponent<Terrain>();
		break;
	case ComponentType::RigidBody:
		AddComponent<RigidBody>();
		break;
	case ComponentType::BoxCollider:
		AddComponent<BoxCollider>();
		break;
	case ComponentType::TerrainCollider:
		AddComponent<TerrainCollider>();
		break;
	case ComponentType::SkyBox:
		AddComponent<SkyBox>();
		break;
	case ComponentType::Light:
		CUR_SCENE->AddLight(AddComponent<Light>());
		break;
	default:
		break;
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(ComponentType type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	return _components[index];
}
shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);
	if (component == nullptr)
	{
		shared_ptr<Transform> transform = make_shared<Transform>();
		AddComponent(transform);
		return transform;
	}
	return static_pointer_cast<Transform>(component);
}

shared_ptr<GameObject> GameObject::Instantiate(const wstring& name)
{
	shared_ptr<GameObject> go = make_shared<GameObject>(name);
	CUR_SCENE->Add(go);
	return go;
}