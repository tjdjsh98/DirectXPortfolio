#pragma once
#include "Component.h"

class Shader;
class Material;

class SkyBox : public Component
{
public:
	SkyBox();
	~SkyBox();

	virtual void Init();
	virtual void Render();

	static ComponentType GetComponentType() { return ComponentType::SkyBox; }

	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	auto GetMaterial() { return _material; }


private:
	shared_ptr<Material> _material;
	shared_ptr<Mesh> _mesh;
};

