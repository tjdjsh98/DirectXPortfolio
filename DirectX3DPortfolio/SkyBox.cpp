#include "pch.h"
#include "SkyBox.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"

SkyBox::SkyBox() : Component(ComponentType::SkyBox)
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Init()
{
	_mesh = RESOURCE->Get<Mesh>(L"Sphere");
	_material = RESOURCE->Get<Material>(L"Sky");
}

void SkyBox::Render()
{
	if (CUR_SCENE->GetIsDrawShadowMap()) return;
	if (_mesh == nullptr || _material == nullptr) return;
	auto& shader = _material->GetShader();
	if (shader == nullptr) return;
	_material->Update();
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	shader->DrawIndexed(L"SkyBoxTech", 0, _mesh->GetIndexBuffer()->GetCount());
}
