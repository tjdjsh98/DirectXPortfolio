#pragma once
#include "Component.h"

enum class ProjectionType
{
	Perspective,
	Orthographic
};

class Camera : public Component, public enable_shared_from_this<Camera>
{
public:
	Camera();
	virtual ~Camera();

	virtual void Init() override;
	virtual void Update() override;
	static ComponentType GetComponentType() { return ComponentType::Camera; }


	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }
	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	ProjectionType GetProjectionType() { return _type; }

	Vector3 ScreenToWorldPosition(Vector4 screenPosition);

public:
	static Matrix S_MatView;
	static Matrix S_MatProjection;

private:
	ProjectionType _type = ProjectionType::Perspective;
	Matrix _matView = Matrix::Identity;
	Matrix _matProjection = Matrix::Identity;

	float _near = 1.0f;
	float _far = 1000.0f;
	float _fov = XM_PI / 4.0f;
	float _width = 0.0f;
	float _height = 0.0f;

public:
	void SortGameObjects();
	void Render_Forward();

private:
	vector<shared_ptr<GameObject>> _vecForward;

};

