#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Type.h"
#include "Define.h"

#include <iostream>
#include <string>
#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h>
#include <malloc.h>
#include <map>
#include <array>
#include <memory>
#include <tchar.h>
#include <stdlib.h>
#include <fstream>

using namespace std;


// DirectX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <d3d11shader.h>
#include <d3dx11effect.h>
#include <DirectXTex.h>
#include <DirectXTex.inl>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_debug.lib")
#pragma comment(lib,"Effects11d.lib")
#pragma comment(lib,"Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib,"Effects11.lib")
#pragma comment(lib,"Assimp/assimp-vc143-mt.lib")
#endif

using namespace Microsoft::WRL;
using namespace DirectX;

// WIN
#include <SDKDDKVer.h>
#include <windows.h>
#include "Resource.h"

// Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

//ImGui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// PhysicsX
#include "PhysicsX/PxPhysicsAPI.h"
#include "PhysicsX/vehicle/PxVehicleSDK.h"

#pragma comment(lib,"PhysX/PhysXCommon_64.lib")
#pragma comment(lib,"PhysX/PhysX_64.lib")
#pragma comment(lib,"PhysX/PhysXExtensions_static_64.lib")
#pragma comment(lib,"PhysX/PhysXCooking_64.lib")
#pragma comment(lib,"PhysX/PhysXVehicle_static_64.lib")
#pragma comment(lib,"PhysX/PhysXVehicle2_static_64.lib")
#pragma comment(lib,"PhysX/SnippetRender_static_64.lib")
#pragma comment(lib,"PhysX/PhysXPvdSDK_static_64.lib")
#pragma comment(lib,"PhysX/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib,"PhysX/SnippetUtils_static_64.lib")
#pragma comment(lib,"PhysX/LowLevel_static_64.lib")
#pragma comment(lib,"PhysX/LowLevelAABB_static_64.lib")
#pragma comment(lib,"PhysX/SimulationController_static_64.lib")
#pragma comment(lib,"PhysX/PhysXFoundation_64.lib")
#pragma comment(lib,"PhysX/PhysXTask_static_64.lib")
#pragma comment(lib,"PhysX/PVDRuntime_64.lib")
#pragma comment(lib,"PhysX/SceneQuery_static_64.lib") 
#pragma comment(lib,"PhysX/LowLevelDynamics_static_64.lib")


using namespace physx;

// Manager
#include "Game.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "InstancingManager.h"
#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "Gizmo.h"


#include "Scene.h"
#include "Game.h"
#include "Graphics.h"
#include"IExcute.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Geometry.h"
#include "VertexData.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "BindShaderDesc.h"
#include "MathUtils.h"