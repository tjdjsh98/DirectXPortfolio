#pragma once
class IExcute
{
public:
	IExcute();
	virtual ~IExcute();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render();
	virtual void Release();
};

