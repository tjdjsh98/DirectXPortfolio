#pragma once
#include "Component.h"

class Shader;

class Light : public Component, public enable_shared_from_this<Light>
{
public:
	Light();
	~Light();
	
	virtual void Init();

	static ComponentType GetComponentType() { return ComponentType::Light; }
	const LightDesc& GetLightDesc() { return _lightDesc; }

	void SetAmbient(Color ambient);
	void SetDiffuse(Color diffuse);
	void SetSpecular(Color specular);
	void SetDIrection(Vector3 direction);
	void SetEmissive(Color emissive);

	void PushData(shared_ptr<Shader> shader);

private:
	LightDesc _lightDesc;
};

