#pragma once
#include "BindShaderDesc.h"

class GameObject;
class Camera;
class Light;
struct LightDesc;

class Scene
{
public:
	Scene();
public:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();
	virtual void LateUpdate();

	virtual void Render();

	virtual void Add(shared_ptr<GameObject> object);
	virtual void Add(const vector<shared_ptr<GameObject>>& object);
	virtual void Remove(shared_ptr<GameObject> objects);

	virtual void AddCamera(shared_ptr<Camera> camera) { 
		if(_cameras.find(camera) == _cameras.end())
			_cameras.insert(camera); }
	virtual void AddLight(shared_ptr<Light> light) {
		if (_lights.find(light) == _lights.end())
		_lights.insert(light); }
	


	unordered_set<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	shared_ptr<Camera> GetMainCamera();
	shared_ptr<Camera> GetUICamera();
	shared_ptr<Light> GetLight() { return _lights.empty() ? nullptr : *_lights.begin(); }
	const bool& GetIsDrawShadowMap() { return _isDrawShadowMap; }
	
	
	void SetDrawShadowMap(bool draw) { _isDrawShadowMap = draw; }

	
	void PickUI();
	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);

	void CheckCollision();

private:
	// 전역변수 전달용 쉐이더
	shared_ptr<Shader> _shader;
	bool _isDrawShadowMap = false;

private:
	unordered_set<shared_ptr<GameObject>> _objects;
	// Cache Camera
	unordered_set<shared_ptr<Camera>> _cameras;
	// Cache Light
	unordered_set<shared_ptr<Light>> _lights;
};

