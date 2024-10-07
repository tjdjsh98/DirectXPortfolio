#pragma once
#include "Component.h"


class Transform;
class MonoBehaviour;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject(const wstring& name);
	~GameObject();

public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	static shared_ptr<GameObject> Instantiate(const wstring& name);

public:
	template<typename T>
	shared_ptr<T> GetCompoent();
	template<typename T>
	shared_ptr<T> AddComponent();

public:
	shared_ptr<Component> GetFixedComponent(ComponentType type);
	void AddComponent(shared_ptr<Component> component);
	void AddComponent(ComponentType type);
	

	shared_ptr<Transform> GetTransform();

	wstring GetName() { return _name; }
	void SetName(const wstring& name) { _name = name; }

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<Component>> _scripts;

	wstring _name;
};

template<typename T>
shared_ptr<T> GameObject::GetCompoent()
{
	for (auto compoent : _components)
	{
		if (compoent == nullptr) continue;
		shared_ptr<T> castComponent = dynamic_pointer_cast<T>(compoent);
		if (castComponent != nullptr) return castComponent;
	}
	for (auto script : _scripts)
	{
		if (script == nullptr) continue;
		shared_ptr<T> castScript = dynamic_pointer_cast<T>(script);
		if (castScript != nullptr) return castScript;
	}
	return nullptr;
}

template<typename T>
inline shared_ptr<T> GameObject::AddComponent()
{
	ComponentType type = T::GetComponentType();

	if (type == ComponentType::Script)
	{
		shared_ptr<T> script = make_shared<T>();
		script->SetGameObject(shared_from_this());
		_scripts.push_back(script);
		script->Init();
		return script;
	}
	else
	{
		uint8 index = static_cast<uint8>(type);

		if (_components[index] != nullptr) return static_pointer_cast<T>(_components[index]);

		
		shared_ptr<T> component = make_shared<T>();
		component->SetGameObject(shared_from_this());
		_components[index] = component;
		component->Init();

		return component;
	}

	return nullptr;
}
