#include "pch.h"
//#include "TerrainEditor.h"
//#include "Terrain.h";
//#include "TerrainPicker.h"
//#include "GameObject.h"
//#include "Camera.h"
//#include "CameraMove.h"
//#include "Transform.h"
//#include "MeshRenderer.h"
//#include "Material.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "RigidBody.h"
//#include "TerrainCollider.h"
//#include "Physics.h"
//#include "Utils.h"
//#include "Model.h"
//#include "Light.h"
//
//
//TerrainEditor::TerrainEditor()
//{
//
//}
//
//TerrainEditor::~TerrainEditor()
//{
//
//}
//
//void TerrainEditor::Init()
//{
//	_shader = RESOURCE->Get<Shader>(L"Terrain");
//	{
//		_camera = GameObject::Instantiate(L"Camera");
//
//		_camera->GetTransform()->SetPosition(Vector3(0, 10, -5));
//		_camera->AddComponent(make_shared<Camera>());
//		_camera->AddComponent(make_shared<CameraMove>());
//		_terrainPicker = _camera->AddComponent<TerrainPicker>();
//		_camera->AddComponent<Light>()->SetDIrection(Vector3(-1, -1, 0));
//	}
//
//	{
//		_terrainObj = GameObject::Instantiate(L"Terrain");
//		_terrain = _terrainObj->AddComponent<Terrain>();
//
//		TerrainDesc desc;
//		{
//			desc.size.x = 128;
//			desc.size.y = 128;
//			desc.cellSpacing = 0.5f;
//			desc.materials.push_back(RESOURCE->Get<Material>(L"Grass"));
//			desc.materials.push_back(RESOURCE->Get<Material>(L"Moss"));
//			desc.materials.push_back(RESOURCE->Get<Material>(L"Rock"));
//			desc.materials.push_back(RESOURCE->Get<Material>(L"SandyGravel"));
//		}
//		_terrain->Create(desc);
//		_terrainObj->AddComponent<TerrainCollider>()->CookingCollider();
//	}
//	{
//		_obj = GameObject::Instantiate(L"Go");
//		_obj->GetTransform()->SetPosition(Vector3(0, 100, 0));
//		;		auto MeshR = _obj->AddComponent<MeshRenderer>();
//		MeshR->SetMesh(RESOURCE->Get<Mesh>(L"Cube"));
//		MeshR->SetMaterial(RESOURCE->Get<Material>(L"Default"));
//		_obj->AddComponent<RigidBody>();
//
//	}
//	
//	_terrainPicker->SetTerrain(_terrain);
//	_terrain->SetShowBrush(true);
//}
//
//void TerrainEditor::Update()
//{
//	GIZMO->DrawCube(Vector3(0, 10, 0), Color(1, 1, 1, 1));
//	GIZMO->DrawLine(Vector3(0, 10, 0), Vector3(-10, 10, 0), Color(0, 1, 0, 1));
//
//}
//
//void TerrainEditor::Render()
//{
//	TestCastCast();
//
//	const Vector3& mousePos = _terrainPicker->GetMousePositionOnTerrain(INPUT->GetMousePos().x, INPUT->GetMousePos().y);
//
//
//	_terrain->SetMousePos(mousePos);
//	_terrain->SetBrushSize(_brushSize);
//
//	ImGui::Begin("Terrain");
//
//	ImGui::Text("Raycast : %d", size);
//
//	if (_terrain->GetIsLockFrustums())
//		ImGui::Text((char*)u8"프러스텀 컬림 잠금 : True");
//	else
//		ImGui::Text((char*)u8"프러스텀 컬림 잠금 : False");
//
//	ImGui::Text("%f %f %f", mousePos.x, mousePos.y, mousePos.z);
//	if (ImGui::BeginTable("split", 6))
//	{
//		ImGui::TableNextColumn(); 
//		if (ImGui::Button((char*)u8"높이", ImVec2(50, 50)))
//		{
//			_paintType = PaintType::Height;
//		}
//		if (ImGui::Button((char*)u8"부드럽게", ImVec2(50, 50)))
//		{
//			_paintType = PaintType::Smooth;
//		}
//
//		for (int i = 0; i < 4; i++) {
//			ImGui::TableNextColumn();
//			if (ImGui::ImageButton(_terrain->GetTextureSRV(i).Get(), ImVec2(50, 50)))
//			{
//				_paintType = PaintType::Texture;
//				_paintIndex = i;
//			}
//		}
//		ImGui::EndTable();
//	}
//	ImGui::SliderFloat((char*)u8"브러쉬크기", &_brushSize, 0.1f, 20.0f);
//	ImGui::SliderFloat((char*)u8"강도", &_brushIntensity, 0.1f, 20.0f);
//
//	char filename[128]{ 0, };
//	ImGui::InputText((char*)u8"파일이름", _filename, sizeof(filename));
//	if (ImGui::Button((char*)u8"저장"))
//	{
//		string str(_filename);
//		_terrain->SaveMapData(str);
//	}
//	if (ImGui::Button((char*)u8"불러오기"))
//	{
//		string str(_filename);
//		_terrain->LoadMapData(str);
//	}
//	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
//		_terrain->SetPass(0);
//	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
//		_terrain->SetPass(1);
//
//	if (!ImGui::IsWindowHovered())
//	{
//		if (INPUT->GetButton(KEY_TYPE::LBUTTON))
//		{
//			_terrainPicker->Pick(mousePos);
//			for (auto index : _terrainPicker->_pickingIndices)
//			{
//				if (_paintType == PaintType::Height) 
//				{
//					_terrain->ModifyHeight(index, INPUT->GetButton(KEY_TYPE::LSHIFT) ? -_brushIntensity : _brushIntensity, _terrainPicker->_point, _brushSize);
//				}
//				if (_paintType == PaintType::Smooth)
//				{
//					_terrain->SmoothHeight(index, _terrainPicker->_point, _brushSize);
//				}
//				else if (_paintType == PaintType::Texture)
//				{
//					_terrain->ModifyBlend(index, _paintIndex, _terrainPicker->_point, _brushSize);
//				}
//			}
//		}
//	}
//	PxVec3 terrainPos = _terrain->GetGameObject()->GetCompoent<TerrainCollider>()->GetPosition();
//	ImGui::Text((char*)u8"위치 %f %f %f", &terrainPos.x, &terrainPos.y, &terrainPos.z);
//
//	ImGui::InputFloat3((char*)u8"위치", (float*)&_resetPos);
//	if (ImGui::Button((char*)u8"위치리셋"))
//	{
//		_obj->GetCompoent<RigidBody>()->SetPosition(_resetPos);
//		_obj->GetTransform()->SetPosition(_resetPos);
//		_obj->GetCompoent<RigidBody>()->SetVelocity(Vector3::Zero);
//
//	}
//
//	ImGui::End();
//
//}
//void TerrainEditor::Resize(uint32 sizeX, uint32 sizeZ)
//{
//	_sizeX = sizeX;
//	_sizeZ = sizeZ;
//	_heightData.resize(sizeX * sizeZ);
//}
//void TerrainEditor::SaveHeightMapData(const wstring& path)
//{
//}
//void TerrainEditor::TestCastCast()
//{
//	auto camera = CUR_SCENE->GetMainCamera();
//	if (camera == nullptr) return;
//
//	float centerX = GAME->GetGameDesc().width / 2;
//	float centerY = GAME->GetGameDesc().height / 2;
//
//	Vector3 cameraPos = camera->ScreenToWorldPosition(Vector4(centerX, centerY, 0, 1));
//	Vector3 direction = camera->ScreenToWorldPosition(Vector4(INPUT->GetMousePos().x, INPUT->GetMousePos().y, 1, 0));
//	direction.Normalize();
//
//	vector<Hit> hits = Physics::Raycast(cameraPos, direction, FLT_MAX);
//
//	size = hits.size();
//	if (hits.size() > 0)
//	{
//		colliderName = hits[0].collider->GetName();
//	}
//	else {
//		colliderName = L"";
//	}
//
//}
//void TerrainEditor::LoadHeightMapData(const wstring& path)
//{
//}
//
