#pragma once
#include "ResourceBase.h"

class Texture;
class Mesh;
class Material;
class Shader;
class AnimationClip;
class Model;

using KeyObjMap = map<wstring, shared_ptr<ResourceBase>>;

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager);

public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	template<typename T>
	ResourceType GetResourceType();

public:
	array<KeyObjMap, RESOURCE_TYPE_COUNT>& GetResources() { return _resources; }
	template<typename T>
	map<wstring, shared_ptr<ResourceBase>> GetResources();

private:
	void CreateDefaultMesh();
	void CreateDefaultShader();

private:
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources;

private:
	const string _resourcePath;
};

template<typename T>
shared_ptr<T> ResourceManager::Load(const wstring& key, const wstring& path)
{
	ResourceType type = GetResourceType<T>();
	auto& keyObj = _resources[static_cast<uint8>(type)];
	
	auto findit = keyObj.find(key);
	if (findit != keyObj.end())
		return static_pointer_cast<T>(findit->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	keyObj[key] = object;

	return object;
}

template<typename T>
bool ResourceManager::Add(const wstring& key, shared_ptr<T> object)
{
	ResourceType type = GetResourceType<T>();
	auto& keyObj = _resources[static_cast<uint8>(type)];

	auto findit = keyObj.find(key);
	if (findit != keyObj.end())
		return false;

	keyObj[key] = object;

	int a = 0;
	return true;
}

template<typename T>
shared_ptr<T> ResourceManager::Get(const wstring& key)
{
	ResourceType type = GetResourceType<T>();
	auto& keyObj = _resources[static_cast<uint8>(type)];

	auto findit = keyObj.find(key);
	if (findit != keyObj.end())
		return static_pointer_cast<T>(findit->second);

	return nullptr;
}

template<typename T>
ResourceType ResourceManager::GetResourceType()
{
	if (std::is_same_v<T, Texture>)
		return ResourceType::Texture;
	if (std::is_same_v<T, Mesh>)
		return ResourceType::Mesh;
	if (std::is_same_v<T, Material>)
		return ResourceType::Material;
	if (std::is_same_v<T, Shader>)
		return ResourceType::Shader;
	if (std::is_same_v<T, Model>)
		return ResourceType::Model;
	if (std::is_same_v<T, AnimationClip>)
		return ResourceType::Animation;
	assert(false);
	return ResourceType::None;
}

template<typename T>
inline map<wstring, shared_ptr<ResourceBase>> ResourceManager::GetResources()
{
	ResourceType type = GetResourceType<T>();

	return _resources[static_cast<uint8>(type)];
}

