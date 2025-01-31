#pragma once

struct ModelBone;
struct ModelMesh;
struct AnimationClip;

class Material;
class GameObject;

class Model : public enable_shared_from_this<Model> , public ResourceBase
{
public:
	Model();
	~Model();

public:
	void ReadMaterial(wstring filename);
	void ReadModel(wstring filename);
	void ReadAnimation(wstring filename);

	uint32 GetMaterialCount() { return static_cast<uint32>(_materials.size()); }
	vector<shared_ptr<Material>>& GetMaterials() { return _materials; }
	shared_ptr<Material> GetMaterialByIndex(uint32 index) { return _materials[index]; }
	shared_ptr<Material> GetMaterialByName(const wstring& name);

	uint32 GetMeshCount() { return static_cast<uint32>(_meshes.size()); }
	vector<shared_ptr<Mesh>>& GetMeshes() { return _meshes; }
	shared_ptr<Mesh> GetMeshByIndex(uint32 index) { return _meshes[index]; }
	shared_ptr<Mesh> GetMeshByName(const wstring& name);

	uint32 GetBoneCount() { return static_cast<uint32>(_bones.size()); }
	vector<shared_ptr<ModelBone>>& GetBones() { return _bones; }
	shared_ptr<ModelBone> GetBoneByIndex(uint32 index) { return (index < 0 || index >= _bones.size() ? nullptr : _bones[index]); }
	shared_ptr<ModelBone> GetBoneByName(const wstring& name);

	uint32 GetAnimationCount() { return _animations.size(); }
	vector<shared_ptr<AnimationClip>>& GetAnimations() { return _animations; }
	shared_ptr<AnimationClip> GetAnimationByIndex(UINT index) { return (index < 0 || index >= _animations.size()) ? nullptr : _animations[index]; }
	shared_ptr<AnimationClip> GetAnimationByName(wstring name);


public:
	vector<shared_ptr<GameObject>> Instantiate() { return _isSkinnedMesh ? InstantiateSkinnedMesh() : InstantiateMesh(); }
	vector<shared_ptr<GameObject>> InstantiateSkinnedMesh();
	vector<shared_ptr<GameObject>> InstantiateMesh();


	void BindCacheInfo();

private:
	wstring _modelPath = L"../Resources/Models/";
	wstring _texturePath = L"../Resources/Textures/";

private:
	shared_ptr<ModelBone> _root;
	vector<shared_ptr<Material>> _materials;
	vector<shared_ptr<ModelBone>> _bones;
	vector<shared_ptr<Mesh>> _meshes;

	bool _isSkinnedMesh = false;
	
	vector<shared_ptr<AnimationClip>> _animations;
};

