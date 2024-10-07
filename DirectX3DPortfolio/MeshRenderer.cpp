#include "pch.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Shader.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "InstancingBuffer.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}


MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::RenderingInstancing(shared_ptr<InstancingBuffer>& buffer)
{
	if (_mesh == nullptr || _material == nullptr) return;
	shared_ptr<Shader> shader = _material->GetShader();

	if (shader == nullptr) return;

	// vertex
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	// InstancingData
	buffer->PushData();


	if (_material)
	{
		_material->Update();
		if(CUR_SCENE->GetIsDrawShadowMap())
			shader->DrawIndexedInstanced(L"MeshTech", 2, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());
		else
			shader->DrawIndexedInstanced(L"MeshTech", 0, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());
	}
}
InstanceID MeshRenderer::GetInstanceID()
{ return make_pair((uint64)_mesh.get(), (uint64)_material.get()); }