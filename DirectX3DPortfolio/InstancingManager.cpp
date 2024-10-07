#include "pch.h"
#include "MeshRenderer.h"
#include "InstancingManager.h"
#include "GameObject.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "InstancingBuffer.h"
#include "Animator.h"


void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
	ClearData();

	RenderMeshRenderer(gameObjects);
	RenderSkinnedMeshRenderer(gameObjects);

}

void InstancingManager::ClearData()
{
	for (auto& pair : _buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second;
		buffer->ClearData();
	}
}

void InstancingManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<InstanceID, vector<shared_ptr<MeshRenderer>>> cache;

	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		shared_ptr<MeshRenderer> mesh = gameObject->GetCompoent<MeshRenderer>();
		if (mesh == nullptr) continue;
		const InstanceID instanceId = mesh->GetInstanceID();
		cache[instanceId].push_back(mesh);
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<MeshRenderer>>& vec = pair.second;

		const InstanceID instanceId = pair.first;

		for(uint32 i= 0 ; i < vec.size(); i++)
		{
			const shared_ptr<MeshRenderer>& mesh = vec[i];
			InstancingData data;
			data.world = mesh->GetTransform()->GetWorldMatrix();

			AddData(instanceId, data);
		}

		vec[0]->RenderingInstancing(_buffers[instanceId]);
	}
}

void InstancingManager::RenderSkinnedMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<InstanceID, vector<shared_ptr<SkinnedMeshRenderer>>> cache;

	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		shared_ptr<SkinnedMeshRenderer> mesh = gameObject->GetCompoent<SkinnedMeshRenderer>();

		if (mesh == nullptr || mesh->GetAnimator() == nullptr) continue;
		const InstanceID instanceId = mesh->GetInstanceID();

		cache[instanceId].push_back(mesh);
	}

	for (auto& pair : cache)
	{
		shared_ptr<InstancedTweenDesc> tweenDesc = make_shared<InstancedTweenDesc>();
		const vector<shared_ptr<SkinnedMeshRenderer>>& vec = pair.second;
		
		const InstanceID instanceId = pair.first;

		for (uint32 i = 0; i < vec.size(); i++)
		{
			const shared_ptr<SkinnedMeshRenderer>& mesh = vec[i];
			InstancingData data;
			data.world = mesh->GetTransform()->GetParent()->GetWorldMatrix();

			AddData(instanceId, data);

			tweenDesc->tweens[i] = mesh->GetAnimator()->GetTweenDesc();
		}
		
		vec[0]->RenderAnimationInstancing(_buffers[instanceId], tweenDesc);
	}
}

void InstancingManager::AddData(InstanceID instanceId, InstancingData& data)
{
	if (_buffers.find(instanceId) == _buffers.end())
		_buffers[instanceId] = make_shared<InstancingBuffer>();

	_buffers[instanceId]->AddData(data);
}
