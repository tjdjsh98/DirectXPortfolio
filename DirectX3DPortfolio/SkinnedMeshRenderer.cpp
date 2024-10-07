#include "pch.h"
#include "SkinnedMeshRenderer.h"
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include "Material.h"
#include "BindShaderDesc.h"
#include "InstancingBuffer.h"
#include "ModelMesh.h"
#include "Animator.h"
#include "GameObject.h"

SkinnedMeshRenderer::SkinnedMeshRenderer() : Component(ComponentType::SkinnedMeshRenderer)
{
}
SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::Render()
{
	if (_mesh == nullptr)
		return;

	if (_animator != nullptr && _animator->GetIsPlay())
		return;

	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	BoneDesc boneDesc;


	for (int b = 0; b < _bones.size(); b++)
	{
		if (b >= _bones.size()) continue;
		if (_bones[b] == nullptr) continue;

		Matrix m = _bones[b]->GetTransform()->GetWorldMatrix();
		Matrix toRootMatrix = _model->GetBoneByIndex(b)->transform;
		Matrix invGlobal = toRootMatrix.Invert();

		Matrix result = invGlobal * m;

		boneDesc.boneTransform[b] = result;
	}
	_material->GetShader()->PushBoneData(boneDesc);

	// TransformBuffer
	TransformDesc desc;
	{
		desc.W = Matrix::Identity;

	}
	_material->GetShader()->PushTransformData(desc);


	if (_material) {
		_material->Update();
		if (CUR_SCENE->GetIsDrawShadowMap())
			_material->GetShader()->DrawIndexed(L"SkinnedMeshTech", 2, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
		else
			_material->GetShader()->DrawIndexed(L"SkinnedMeshTech", 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
	}
}
// 애니메이션이 있을 경우에만
void SkinnedMeshRenderer::RenderAnimationInstancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstancedTweenDesc>& tweenDesc)
{
	if (_model == nullptr)
		return;

	if (_animator == nullptr || !_animator->GetIsPlay())
		return;

	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_material->GetShader()->PushTweenData(*tweenDesc.get());


	_animator->PushAnimationData(_material->GetShader());
	
	_mesh->material->Update();



	buffer->PushData();
	if (CUR_SCENE->GetIsDrawShadowMap())
		_material->GetShader()->DrawIndexedInstanced(L"SkinnedMeshTech", 3, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());
	else
		_material->GetShader()->DrawIndexedInstanced(L"SkinnedMeshTech", 1, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());
}
