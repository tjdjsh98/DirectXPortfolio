#pragma once
#include "Component.h"

class Mesh;
class Material;
class Shader;

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();
	static ComponentType GetComponentType() { return ComponentType::MeshRenderer; }

	shared_ptr<Mesh> GetMesh() { return _mesh; }
	shared_ptr<Material> GetMaterial() { return _material;}

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetPass(uint8 pass) { _pass = pass; }

	uint8 GetPass() { return _pass; }
	void RenderingInstancing(shared_ptr<class InstancingBuffer>& buffer);

	InstanceID GetInstanceID(); 

private:
	shared_ptr<Mesh> _mesh = nullptr;
	shared_ptr<Material> _material = nullptr;
	uint8 _pass = 0;

};

