#pragma once

class Graphics;

struct GameDesc
{
	class IExcute* app = nullptr;
	HINSTANCE hInstance = 0;
	wstring appName = L"Game";
	HWND hWnd = 0;
	float width = 800;
	float height = 600;
	Color clearColor = { 0,1,1,0 };
};

class Game
{
	DECLARE_SINGLE(Game);

public:
	~Game();

public:
	WPARAM Run(GameDesc desc);

public:
	shared_ptr<Graphics> GetGraphics() { return _graphics; }
	const GameDesc& GetGameDesc() { return _desc; }
private:
	ATOM MyRegisterClass();
	BOOL InitInstance(int nCmdShow);

	void Update();

	void LoadFont();
	void ShowFps();
private:
	GameDesc _desc = {};
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	shared_ptr<Graphics> _graphics = nullptr;
	ImFont* _font;
};

