#include "pch.h"
#include "Transform.h"
#include "GameObject.h"
#include "RigidBody.h"

Transform::Transform() : Super(ComponentType::Transform)
{

}

Transform::~Transform()
{

}

void Transform::Awake()
{
}

void Transform::Update()
{
}

Vector3 Transform::ToEulerAngles(Quaternion q)
{
	Vector3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);
	_matLocal = matScale * matRotation * matTranslation;

	if (HasParent())
	{
		Matrix parentMatrix = _parent.lock()->GetWorldMatrix();
		_matWorld = _matLocal * parentMatrix;
		Quaternion quat;
		_matWorld.Decompose(_scale, quat, _position);
		_rotation = ToEulerAngles(quat);
	}
	else
	{
		_matWorld = _matLocal;
		_rotation = _localRotation;
		Quaternion quat;
		_matWorld.Decompose(_scale, quat, _position);
	}



	// Children
	for (const shared_ptr<Transform>& child : _children)
		child->UpdateTransform();
}


void Transform::SetLocalMatrix(const Matrix& matrix,bool noUpdate)
{
	_matLocal = matrix;
	Quaternion qut;
	_matLocal.Decompose(_localScale, qut, _localPosition);

	_localRotation = ToEulerAngles(qut);

	if (noUpdate == false) 
	{

		UpdateTransform();
	}
	else 
	{
		if (HasParent())
		{
			Matrix parentMatrix = _parent.lock()->GetWorldMatrix();
			_matWorld = _matLocal * parentMatrix;
			Quaternion quat;
			_matWorld.Decompose(_scale, quat, _position);
			_rotation = ToEulerAngles(quat);
		}
		else
		{
			_matWorld = _matLocal;
			_rotation = _localRotation;
			Quaternion quat;
			_matWorld.Decompose(_scale, quat, _position);
		}
	}

}

void Transform::SetScale(const Vector3& worldScale)
{
	if (HasParent())
	{
		Vector3 parentScale = _parent.lock()->GetScale();
		Vector3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Vector3& worldRotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = _parent.lock()->GetWorldMatrix().Invert();

		Vector3 rotation;
		rotation = rotation.TransformNormal(worldRotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
		SetLocalRotation(worldRotation);
}

void Transform::SetRotation(const Quaternion& quat)
{
	Vector3 rot = ToEulerAngles(quat);
	SetRotation(rot);
}

void Transform::SetPosition(const Vector3& worldPosition)
{
	if (HasParent())
	{
		Matrix parentWorldMatrix = _parent.lock()->GetWorldMatrix();
		Matrix worldToParentLocalMatrix = parentWorldMatrix.Invert();

		Vector3 position;
		position = position.Transform(worldPosition, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::SetWorldMatrix(Matrix matWorld)
{
	Vector3 position;
	Quaternion qut;
	Vector3 scale;
	matWorld.Decompose(scale, qut, position);

	SetScale(scale);
	SetRotation(ToEulerAngles(qut));
	SetPosition(position);

}

void Transform::SetParent(shared_ptr<Transform> parent)
{
	if (_parent.lock() != nullptr)
	{
		for(int i = 0 ; i < parent->_children.size(); i++)
		{
			if (parent->_children[i] == shared_from_this())
			{
				parent->_children.erase(parent->_children.begin() + i);
				break;
			}
		}
		_parent.lock() = nullptr;
	}
	_parent = parent;
	if(parent != nullptr)
		parent->AddChild(shared_from_this());

}