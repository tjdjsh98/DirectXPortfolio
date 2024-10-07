#include "pch.h"
#include "Light.h"
#include "Shader.h"

Light::Light() : Component(ComponentType::Light)
{
}

Light::~Light()
{
}

void Light::Init()
{
	CUR_SCENE->AddLight(shared_from_this());
}

void Light::SetAmbient(Color ambient)
{
	_lightDesc.ambient = ambient;
}

void Light::SetDiffuse(Color diffuse)
{
	_lightDesc.diffuse = diffuse;
}

void Light::SetSpecular(Color specular)
{
	_lightDesc.specular = specular;
}

void Light::SetDIrection(Vector3 direction)
{
	_lightDesc.direction = direction;
}

void Light::SetEmissive(Color emissive)
{
	_lightDesc.emissve = emissive;
}

void Light::PushData(shared_ptr<Shader> shader)
{
	shader->PushLightData(_lightDesc);
}
