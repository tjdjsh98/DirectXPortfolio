#pragma once

class GameObject;
class Transform;


enum class ComponentType : uint8
{
	Transform,
	MeshRenderer,
	Camera,
	SkinnedMeshRenderer,
	Animator,
	Terrain,
	RigidBody,
	BoxCollider,
	TerrainCollider,
	SkyBox,
	Light,
	//...

	Script,
	End,
};
enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8> (ComponentType::End)-1
};


extern string ComponentNames[FIXED_COMPONENT_COUNT];

class GameObject;

class Component
{
public:
	Component(ComponentType type);
	virtual ~Component();

public:
	virtual void Init();
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void Render();
	virtual void LateUpdate();
	virtual void FixedUpdate();


public:
	static ComponentType GetComponentType() { return ComponentType::End; }
	ComponentType GetType() { return _type; }
	shared_ptr<GameObject> GetGameObject() { return _gameObject.lock(); }
	shared_ptr<Transform> GetTransform();
	

private:
	friend class GameObject;
		void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	ComponentType _type;
	weak_ptr<GameObject> _gameObject;
};

