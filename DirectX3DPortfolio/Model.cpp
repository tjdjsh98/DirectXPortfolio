#include "pch.h"
#include "Model.h"
#include "Utils.h"
#include "FileUtils.h"
#include "tinyxml2.h"
#include <filesystem>
#include "Material.h"
#include "ModelMesh.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Texture.h"
#include "Shader.h"
#include "Component.h"
#include "SkinnedMeshRenderer.h"
#include "AnimationClip.h"
#include "Animator.h"


Model::Model() : ResourceBase(ResourceType::Mesh)
{

}

Model::~Model()
{

}

void Model::ReadMaterial(wstring filename)
{
	wstring fullPath = _texturePath + filename + L".xml";
	auto parentPath = filesystem::path(fullPath).parent_path();

	tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = document->FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{
		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		wstring materailName = Utils::ToWString(node->GetText());
		shared_ptr<Material> material = RESOURCE->Get<Material>(materailName);


		if (material == nullptr) {
			material = make_shared<Material>();


			material->SetName(materailName);

			// Diffuse Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				wstring textureStr = Utils::ToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
					material->SetDiffuseMap(texture);
				}
			}

			// Specular Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				wstring texture = Utils::ToWString(node->GetText());
				if (texture.length() > 0)
				{
					wstring textureStr = Utils::ToWString(node->GetText());
					if (textureStr.length() > 0)
					{
						shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
						material->SetSpecularMap(texture);
					}
				}
			}

			// Normal Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				wstring textureStr = Utils::ToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
					material->SetNormalMap(texture);
				}
			}

			// Ambient
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->GetMaterialDesc().ambient = color;
			}

			// Diffuse
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->GetMaterialDesc().diffuse = color;
			}

			// Specular
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->GetMaterialDesc().specular = color;
			}

			// Emissive
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->GetMaterialDesc().emissive = color;
			}

			RESOURCE->Add(materailName, material);
		}
		else
		{
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();
			node = node->NextSiblingElement();

		}
		material->SetShader(RESOURCE->Get<Shader>(L"Default"));
		_materials.push_back(material);
		// Next Material
		materialNode = materialNode->NextSiblingElement();
	}

	BindCacheInfo();
}

void Model::ReadModel(wstring filename)
{
	wstring fullPath = _modelPath + filename + L".mesh";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// Bones
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelBone> bone = make_shared<ModelBone>();
			bone->index = file->Read<int32>();
			bone->name = Utils::ToWString(file->Read<string>());
			bone->parentIndex = file->Read<int32>();
			bone->transform = file->Read<Matrix>();

			_bones.push_back(bone);
		}
	}

	// Mesh
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			wstring meshName = Utils::ToWString(file->Read<string>());

			shared_ptr<Mesh> mesh = RESOURCE->Get<Mesh>(meshName);

			if (mesh == nullptr)
			{
				mesh = make_shared<Mesh>();

				mesh->SetName(meshName);
				mesh->SetBoneIndex(file->Read<int32>());

				// Material
				mesh->SetModelMaterialName(Utils::ToWString(file->Read<string>()));

				//VertexData
				{
					const uint32 count = file->Read<uint32>();
					vector<ModelVertexType> vertices;
					vertices.resize(count);

					void* data = vertices.data();
					file->Read(&data, sizeof(ModelVertexType) * count);
					mesh->GetModelGeometry()->AddVertices(vertices);
				}

				//IndexData
				{
					const uint32 count = file->Read<uint32>();

					vector<uint32> indices;
					indices.resize(count);

					void* data = indices.data();
					file->Read(&data, sizeof(uint32) * count);
					mesh->GetModelGeometry()->AddIndices(indices);
				}
				mesh->CreateBuffers();
			}
			RESOURCE->Add(meshName, mesh);
			_meshes.push_back(mesh);
		}
	}

	if (_meshes.size() == 1)
		_isSkinnedMesh = true;
	
	BindCacheInfo();
}

void Model::ReadAnimation(wstring filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);
	wstring animationName = Utils::ToWString(file->Read<string>());
	shared_ptr<AnimationClip> animation = RESOURCE->Get<AnimationClip>(filename);

	if(animation == nullptr)
	{
		animation = make_shared<AnimationClip>();
		animation->SetName(filename);

		AnimationDesc desc;

		desc.name = filename;

		desc.duration = file->Read<float>();
		desc.frameRate = file->Read<float>();
		desc.frameCount = file->Read<uint32>();

		uint32 keyframesCount = file->Read<uint32>();

		for (uint32 i = 0; i < keyframesCount; i++)
		{
			shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
			keyframe->boneName = Utils::ToWString(file->Read<string>());

			uint32 size = file->Read<uint32>();

			if (size > 0)
			{
				keyframe->transforms.resize(size);
				void* ptr = &keyframe->transforms[0];
				file->Read(&ptr, sizeof(ModelKeyframeData) * size);
			}

			desc.keyframes[keyframe->boneName] = keyframe;
		}
		animation->SetAnimationDesc(desc);

		RESOURCE->Add(filename, animation);
	}

	_animations.push_back(animation);
}

std::shared_ptr<Material> Model::GetMaterialByName(const wstring& name)
{
	for (auto& material : _materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return nullptr;
}

std::shared_ptr<Mesh> Model::GetMeshByName(const wstring& name)
{
	for (auto& mesh : _meshes)
	{
		if (mesh->GetName() == name)
			return mesh;
	}

	return nullptr;
}

std::shared_ptr<ModelBone> Model::GetBoneByName(const wstring& name)
{
	for (auto& bone : _bones)
	{
		if (bone->name == name)
			return bone;
	}

	return nullptr;
}

std::shared_ptr<AnimationClip> Model::GetAnimationByName(wstring name)
{
	for (auto& animation : _animations)
	{
		if (animation->GetAnimationDesc().name == name)
			return animation;
	}

	return nullptr;
}
void CollectBoneGameObject(vector<shared_ptr<GameObject>>& bones, shared_ptr<GameObject>& parent, shared_ptr<ModelBone>& bone)
{
	for (auto subData : bone->children)
	{
		shared_ptr<GameObject> subBone = GameObject::Instantiate(subData->name);
		subBone->GetTransform()->SetParent(parent->GetTransform());
		subBone->GetTransform()->SetWorldMatrix(subData->transform);
		
		CollectBoneGameObject(bones,subBone, subData);


		bones[subData->index]=subBone;
	}
}

void Model::BindCacheInfo()
{
	// Mesh에 Material 캐싱
	for (const auto& mesh : _meshes)
	{
		// 이미 찾았으면 스킵
		if (mesh->material != nullptr)
			continue;

		mesh->material = GetMaterialByName(mesh->GetModelMaterialName());
	}

	// Mesh에 Bone 캐싱
	for (const auto& mesh : _meshes)
	{
		// 이미 찾았으면 스킵
		if (mesh->bone != nullptr)
			continue;

		mesh->bone = GetBoneByIndex(mesh->GetBoneIndex());
	}

	// Bone 계층 정보 채우기
	if (_root == nullptr && _bones.size() > 0)
	{
		_root = _bones[0];

		for (const auto& bone : _bones)
		{
			if (bone->parentIndex >= 0)
			{
				bone->parent = _bones[bone->parentIndex];
				bone->parent->children.push_back(bone);
			}
			else
			{
				bone->parent = nullptr;
			}
		}
	}
}

vector<shared_ptr<GameObject>> Model::InstantiateSkinnedMesh()
{
	vector<shared_ptr<GameObject>> instancedObjs;
	int count = GetBoneCount();
	instancedObjs.resize(count);
	shared_ptr<ModelBone> boneData = _bones[0];
	shared_ptr<GameObject> rootBone = GameObject::Instantiate(boneData->name);
	instancedObjs[0] = rootBone;

	for (int i = 1; i < _bones.size(); i++)
	{
		shared_ptr<GameObject> gameObject = GameObject::Instantiate(_bones[i]->name);
		instancedObjs[i] = gameObject;

		if (_bones[i]->parentIndex != -1)
		{
			auto parent = instancedObjs[_bones[i]->parentIndex]->GetTransform();
			gameObject->GetTransform()->SetParent(parent);
		}
		else
		{
			gameObject->GetTransform()->SetParent(rootBone->GetTransform());
		}
		gameObject->GetTransform()->SetWorldMatrix(_bones[i]->transform);
	}


	for (uint32 i = 0; i < GetMeshCount(); i++)
	{
		shared_ptr<Mesh> mesh = GetMeshByIndex(i);
		shared_ptr<Shader> defaultShader = RESOURCE->Get<Shader>(L"Default");
		shared_ptr<Material> material = GetMaterialByName(mesh->GetModelMaterialName());
		material->SetShader(defaultShader);

		if (mesh != nullptr)
		{
			shared_ptr<GameObject> meshGameObject = GameObject::Instantiate(mesh->GetName());
			shared_ptr<SkinnedMeshRenderer> skinnedMeshRenderer = make_shared<SkinnedMeshRenderer>();
			skinnedMeshRenderer->SetModel(shared_from_this());
			skinnedMeshRenderer->SetMesh(mesh);
			skinnedMeshRenderer->SetBone(instancedObjs);
			skinnedMeshRenderer->SetMaterial(material);
			skinnedMeshRenderer->SetPass(1);
			meshGameObject->AddComponent(skinnedMeshRenderer);

			meshGameObject->GetTransform()->SetParent(rootBone->GetTransform());

			instancedObjs.push_back(meshGameObject);
		}
	}

	if (GetAnimationCount() > 0)
	{
		auto animator = rootBone->AddComponent<Animator>();
		animator->SetModel(shared_from_this());

	}

	return instancedObjs;
}
vector<shared_ptr<GameObject>> Model::InstantiateMesh()
{
	int count = GetBoneCount();
	vector<shared_ptr<GameObject>> instancedObjs(count);
	shared_ptr<ModelBone> boneData = _bones[0];
	shared_ptr<GameObject> rootBone = GameObject::Instantiate(boneData->name);
	instancedObjs[0] = rootBone;

	for (int i = 1; i < _bones.size(); i++)
	{
		shared_ptr<GameObject> gameObject = GameObject::Instantiate(_bones[i]->name);
		instancedObjs[i] = gameObject;

		if (_bones[i]->parentIndex != -1)
		{
			auto parent = instancedObjs[_bones[i]->parentIndex]->GetTransform();
			gameObject->GetTransform()->SetParent(parent);
		}
		else
		{
			gameObject->GetTransform()->SetParent(rootBone->GetTransform());
		}
		gameObject->GetTransform()->SetWorldMatrix(_bones[i]->transform);

		auto mesh = RESOURCE->Get<Mesh>(_bones[i]->name);
		if (mesh)
		{
			auto material = RESOURCE->Get<Material>(mesh->GetModelMaterialName());
			auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
			meshRenderer->SetMaterial(material);
			meshRenderer->SetMesh(mesh);
		}
	}

	return instancedObjs;
}
