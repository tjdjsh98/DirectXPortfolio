#pragma once

class InstancingBuffer;
class GameObject;

struct InstancingData;

class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	void Render(vector<shared_ptr<GameObject>>& gameObjects);
	void Clear() { _buffers.clear(); };
	void ClearData();

private:
	void RenderMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects);
	void RenderSkinnedMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects);

private:
	void AddData(InstanceID instanceId, InstancingData& data);

private:
	map<InstanceID, shared_ptr<InstancingBuffer>> _buffers;
};

