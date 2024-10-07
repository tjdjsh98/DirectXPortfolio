#pragma once
#include "Component.h"

class Model;
class Shader;
class Material;
class Animator;

class SkinnedMeshRenderer : public Component
{
public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	static ComponentType GetComponentType() { return ComponentType::SkinnedMeshRenderer; }

	bool GetRenderInstance() { return _animator != nullptr; }


	void SetModel(shared_ptr<Model> model) { _model = model; }
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetPass(uint8 pass) { _pass = pass; }

	virtual void Render();
	void SetBone(vector<shared_ptr<GameObject>> bones) { _bones = bones; }
	void RenderAnimationInstancing(shared_ptr<class InstancingBuffer>& buffer, shared_ptr<InstancedTweenDesc>& tweenDesc);
	InstanceID GetInstanceID() { return make_pair((uint64)_mesh.get(), (uint64)_material.get()); }

	void SetAnimator(shared_ptr<Animator> animator) {_animator = animator;}
	shared_ptr<Animator>& GetAnimator() { return _animator; }


private:
	shared_ptr<Model> _model;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	vector<shared_ptr<GameObject>> _bones;
	
	uint8				_pass = 0;

	shared_ptr<Animator>_animator;
};

