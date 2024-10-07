#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public : 
	MonoBehaviour();
	~MonoBehaviour();

	static ComponentType GetComponentType() { return ComponentType::Script; }

};

