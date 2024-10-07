#include "pch.h"
#include "Game.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "Shader.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"

Game::~Game()
{
}
void Game::LoadFont()
{
    ImGuiIO& io = ImGui::GetIO();
    string font_file = "../Resources\\Fonts\\NanumGothicBold.ttf";

    _font = io.Fonts->AddFontFromFileTTF(font_file.c_str(), 15.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    IM_ASSERT(_font != NULL);
}

WPARAM Game::Run(GameDesc desc)
{
   /* UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);*/

    //// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_DIRECTX3DPORTFOLIO, szWindowClass, MAX_LOADSTRING);

    _desc = desc;

    MyRegisterClass();

    // ���ø����̼� �ʱ�ȭ�� �����մϴ�:
    if (!InitInstance(SW_SHOWNORMAL))           // ȭ�� ������� ������ ����
    //if (!InitInstance(SW_SHOWMINIMIZED))      // �ּ�ȭ�� ����
    {
        return FALSE;
    }


    _graphics = make_shared<Graphics>();
    _graphics->Init(_desc);

    // Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    LoadFont();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(GAME->GetGameDesc().hWnd);
    ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());

    RESOURCE->Init();
    TIME->Init();
    INPUT->Init(_desc.hWnd);
    PHYSICS->Init();
    // ���� ���� ������
    SCENE->Init();

    _desc.app->Init();

    

    MSG msg = { 0 };

    // �⺻ �޽��� �����Դϴ�:
    while (msg.message != WM_QUIT)
    {
        if(::PeekMessage(&msg, NULL,0,0,PM_REMOVE))
        {
            ::TranslateMessage(&msg);       // Peek���� ������ �޽����� ó��
            ::DispatchMessage(&msg);        // WndProc�� �޽��� �����ֱ�
        }
        else
        {
            Update();
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    return msg.wParam;
}



void Game::Update()
{
    TIME->Update();
    INPUT->Update();
    PHYSICS->Update();
    ShowFps();

    _desc.app->Update();
    CUR_SCENE->Update();

    _graphics->BeginRender();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    _desc.app->Render();
    CUR_SCENE->Render();
    GIZMO->Draw();
    
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    _graphics->EndRender();
}
void Game::ShowFps()
{
    uint32 fps = GET_SINGLE(TimeManager)->GetFps();

    WCHAR text[100] = L"";
    ::wsprintf(text, L"FPS : %d", fps);

    ::SetWindowText(_desc.hWnd, text);

}

ATOM Game::MyRegisterClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _desc.hInstance;
    wcex.hIcon = LoadIcon(_desc.hInstance, MAKEINTRESOURCE(IDI_DIRECTX3DPORTFOLIO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DIRECTX3DPORTFOLIO);
    wcex.lpszClassName = _desc.appName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL Game::InitInstance(int nCmdShow)
{
    RECT windowRect = { 0,0,_desc.width, _desc.height };
    ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    _desc.hWnd = CreateWindowW(_desc.appName.c_str(), _desc.appName.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, _desc.hInstance, nullptr);


    if (!_desc.hWnd)
    {
        return FALSE;
    }

    ::ShowWindow(_desc.hWnd, nCmdShow);
    ::UpdateWindow(_desc.hWnd);

    return TRUE;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_SIZE:
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
}

