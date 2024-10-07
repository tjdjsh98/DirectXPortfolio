#pragma once
#include "Component.h"


class Transform : public Component, public enable_shared_from_this<Transform>
{
	using Super = Component;
public:
	Transform();
	~Transform();

	virtual void Awake() override;
	virtual void Update() override;

	void UpdateTransform();

	static ComponentType GetComponentType() { return ComponentType::Transform; }
	
	static Vector3 ToEulerAngles(Quaternion q);

	// Local
	Vector3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vector3 localScale) { _localScale = localScale; UpdateTransform(); }
	Vector3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Vector3 localRotation) { _localRotation = localRotation; UpdateTransform(); }
	Vector3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vector3 localPosition) { _localPosition = localPosition; UpdateTransform(); }

	void SetLocalMatrix(const Matrix& matrix, bool noUpdate = false/* 애니메이션 최적화를 위해 사용*/);
	Matrix GetLocalMatrix() { return _matLocal; }

	// World
	Vector3 GetScale() { return _scale; }
	void SetScale(const Vector3& scale);
	Vector3 GetRotation() { return _rotation; }
	void SetRotation(const Vector3& rotation);
	void SetRotation(const Quaternion& quat);
	const Vector3& GetPosition() { return _position; }
	void SetPosition(const Vector3& position);
	
	Vector3 GetRight() { return _matWorld.Right(); }
	Vector3 GetUp() { return _matWorld.Up(); }
	Vector3 GetLook() { return _matWorld.Backward(); }

	void SetWorldMatrix(Matrix matWorld);
	Matrix GetWorldMatrix() { return _matWorld; }

	// 계층 관계
	bool HasParent() { return _parent.lock() != nullptr; }

	shared_ptr<Transform> GetParent() { return _parent.lock(); }
	void SetParent(shared_ptr<Transform> parent);

	vector<shared_ptr<Transform>> GetChildren() { 
		vector<shared_ptr<Transform>> child(_children.begin(), _children.end());
		return  child;
	}

private:
	void AddChild(shared_ptr<Transform> child) { _children.push_back(child); }
private:
	Vector3 _localScale = { 1.f, 1.f, 1.f };
	Vector3 _localRotation = { 0.f, 0.f, 0.f };
	Vector3 _localPosition = { 0.f, 0.f, 0.f };

	// Cache
	Matrix _matLocal = Matrix::Identity;
	Matrix _matWorld = Matrix::Identity;

	Vector3 _scale = { 1.f, 1.f, 1.f };
	Vector3 _rotation = { 0.f, 0.f, 0.f };
	Vector3 _position = { 0.f, 0.f, 0.f };

private:
	weak_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _children;
};

