#include "pch.h"
#include <Game.h>
#include "EditorTool.h"
#include <crtdbg.h>
#include <Windows.h>
#include "TestDemo.h"
#include "TerrainEditor.h"
#include "EditorWindow.h"

#pragma comment(lib,"Assimp/assimp-vc143-mt.lib")
#pragma comment(lib,"Assimp/assimp-vc143-mtd.lib")


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    GameDesc desc;
    desc.hInstance = hInstance;
    desc.appName = L"Game";
    desc.width = 1500;
    desc.height = 1000;
    //desc.app = new EditorTool();
    desc.app = new TestDemo();
    //desc.app = new TerrainEditor();
    //desc.app = new EditorWindow();
    GAME->Run(desc);

    return 0;
}