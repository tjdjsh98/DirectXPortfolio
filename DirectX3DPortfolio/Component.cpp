#include "pch.h"
#include "Component.h"
#include "GameObject.h"

extern string ComponentNames[FIXED_COMPONENT_COUNT]
{
		"Transform",
		"MeshRenderer",
		"Camera",
		"SkinnedMeshRenderer",
		"Animator",
		"Terrain",
		"RigidBody",
		"BoxCollider",
		"TerrainCollider",
		"SkyBox",
		"Light"
};

Component::Component(ComponentType type) : _type(type)
{
}

Component::~Component()
{
}

void Component::Init()
{
}

void Component::Awake()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::Render()
{
}

void Component::LateUpdate()
{
}

void Component::FixedUpdate()
{
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}
