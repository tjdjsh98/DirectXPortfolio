#define _CRTDBG_MAP_ALLOC
#include "pch.h"
#include "Game.h"
#include <crtdbg.h>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#endif


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    //_CrtSetBreakAlloc(101530);
    GameDesc desc;
    desc.hInstance = hInstance;
    desc.appName = L"Game";
    desc.width = 1080;
    desc.height = 720;
    //desc.app = new TestDemo();

    GAME->Run(desc);

//#if defined(DEBUG) || defined(_DEBUG)
//    Microsoft::WRL::ComPtr<ID3D11Debug> dxgiDebug;
//
//    if (SUCCEEDED(DEVICE->QueryInterface(IID_PPV_ARGS(&dxgiDebug))))
//    {
//        dxgiDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//        dxgiDebug = nullptr;
//    }
//#endif
    
    /*_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();*/

    return 0;
}