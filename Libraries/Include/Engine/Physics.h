#pragma once

struct Hit;
class GameObject;

class Physics
{
public:
	static vector<Hit> Raycast(Vector3 origin, Vector3 direction, float distance);
};

struct Hit
{
	shared_ptr<GameObject> collider;
	Vector3 point;
	Vector3 normal;
	float distance;
};
