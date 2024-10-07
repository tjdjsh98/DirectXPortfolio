#include "pch.h"
#include "TestDemo.h"
#include "Shader.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "CameraMove.h"
#include "Model.h"
#include "ModelMesh.h"
#include "Utils.h"
#include <format>
#include <SceneManager.h>
#include <Scene.h>
#include <Animator.h>
#include <AnimationClip.h>
#include <SkinnedMeshRenderer.h>
#include <Terrain.h>
#include <RigidBody.h>
#include <TerrainPicker.h>
#include <SkyBox.h>
#include <ShadowMap.h>
#include <CharacterController.h>
#include <Material.h>
#include <Mesh.h>
#include <Model.h>
#include <Light.h>
#include <TerrainCollider.h>
#include <BoxCollider.h>


extern string ComponentNames[FIXED_COMPONENT_COUNT];

TestDemo::TestDemo()
{
	_terrainDesc = new TerrainDesc();
	_terrainDesc->materials.resize(4);
}

TestDemo::~TestDemo()
{
	delete _terrainDesc;
}


void TestDemo::Init()
{
	_shadowMap = make_shared<ShadowMap>();
	_shader = RESOURCE->Get<Shader>(L"Default");
	_terrainPicker = make_shared<TerrainPicker>();
	
	//Camera
	{
		shared_ptr<GameObject> camera = GameObject::Instantiate(L"Camera");

		_camera = camera;
		_camera->GetTransform()->SetPosition(Vector3(0, 0, -5));
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraMove>());
		_picker = camera->AddComponent<TerrainPicker>();

	}
	
	// Model
	{
		_character = GameObject::Instantiate(L"전화부스");

		_model = RESOURCE->Get<Model>(L"TelephoneBooth");
			
		auto meshes = _model->InstantiateMesh();
		meshes[0]->GetTransform()->SetParent(_character->GetTransform());
			
		_character->GetTransform()->SetLocalScale(_character->GetTransform()->GetLocalScale() * 0.01f);

	}

	//{
	//	auto kachujin = GameObject::Instantiate(L"Kachujin");
	//	
	//	auto model = RESOURCE->Get<Model>(L"Kachujin");
	//	auto skinnedMesh = model->InstantiateSkinnedMesh();
	//	skinnedMesh[0]->GetTransform()->SetParent(kachujin->GetTransform());
	//	
	//	auto animator = skinnedMesh[0]->AddComponent<Animator>();
	//	animator->SetModel(model);
	//	kachujin->GetTransform()->SetLocalScale(Vector3(0.01f, 0.01f, 0.01f));
	//	kachujin->AddComponent<RigidBody>();
	//	kachujin->AddComponent<CharacterController>();
	//}

	////// Terrain
	////{
	////	auto terrain = GameObject::Instantiate(L"Terrain");
	////	_terrain = terrain->AddComponent<Terrain>();
	////	auto material =  make_shared<Material>();
	////	material->SetShader(_shader);
	////	//_terrain->Create(2049, 2049, material);
	////}

}
void TestDemo::Update()
{
	_shadowMap->BindDsvAndSetNullRenderTarget();
	_shadowMap->BakeShadowMap();

}
void TestDemo::Render()
{
	ShowHierarchy();
	ShowInspector();
	ShowGameObjectCreator();
	ShowModels();
}

void TestDemo::DisplayNode(shared_ptr<GameObject> parent)
{
	for (int i = 0; i < parent->GetTransform()->GetChildren().size(); i++)
	{
		shared_ptr<Transform> tr = parent->GetTransform()->GetChildren()[i];
		shared_ptr<GameObject> obj = parent->GetTransform()->GetChildren()[i]->GetGameObject();

		ImGuiTreeNodeFlags nodeFlag = 0;
		if (tr->GetChildren().size() > 0)
			nodeFlag |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;
		else
			nodeFlag |= ImGuiTreeNodeFlags_Leaf;

		if (_selectedGameObject == obj)
			nodeFlag |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

		wstring text = obj->GetName();
		char name[256];
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, name, 256, NULL, NULL);
		if (ImGui::TreeNodeEx(name, nodeFlag))
		{
			if (ImGui::IsItemClicked() && !_clicked)
			{
				_selectedGameObject = obj;
				_clicked = true;
			}
			DisplayNode(obj);
			ImGui::TreePop();
		}
		
		if (ImGui::IsItemClicked() && !_clicked)
		{
			_selectedGameObject = obj;
			_clicked = true;
		}
	}
}

void TestDemo::OpenMeshSelection()
{
	ImGui::OpenPopup("Mesh");
}
shared_ptr<Mesh> TestDemo::ShowMeshSelection()
{
	auto map = RESOURCE->GetResources<Mesh>();

	shared_ptr<Mesh> result = nullptr;

	if (ImGui::BeginPopup("Mesh"))
	{

		for (auto mesh : map)
		{
			if (ImGui::Selectable(Utils::ToUtf8(mesh.first).c_str()))
			{
				result = static_pointer_cast<Mesh>(mesh.second);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return result;
			}
		}

		ImGui::EndPopup();
	}

	return result;
}

void TestDemo::OpenMaterialSelection()
{
	ImGui::OpenPopup("Material");
}

shared_ptr<Material> TestDemo::ShowMaterialSelection()
{
	auto map = RESOURCE->GetResources<Material>();

	shared_ptr<Material> result = nullptr;

	if (ImGui::BeginPopup("Material"))
	{

		for (auto material : map)
		{
			if (ImGui::Selectable(Utils::ToUtf8(material.first).c_str()))
			{
				result = static_pointer_cast<Material>(material.second);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return result;
			}
		}

		ImGui::EndPopup();
	}

	return result;
}

void TestDemo::ShowHierarchy()
{
	_clicked = false;
	auto hierarchyGameObjects = CUR_SCENE->GetObjects();
	ImGui::Begin("Hierarchy");
	for (auto go : hierarchyGameObjects)
	{
		if (!go->GetTransform()->HasParent())
		{
			ImGuiTreeNodeFlags nodeFlag = 0;
			if (go->GetTransform()->GetChildren().size()>0)
				nodeFlag |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;
			else
				nodeFlag |= ImGuiTreeNodeFlags_Leaf;

			if (_selectedGameObject == go)
				nodeFlag |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

			wstring text = go->GetName();
			char name[256];
			WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, name, 256, NULL, NULL);
			if (ImGui::TreeNodeEx(name, nodeFlag))
			{
				if (ImGui::IsItemClicked() && !_clicked)
				{
					_selectedGameObject = go;
					_clicked = true;
				}

				DisplayNode(go);
				ImGui::TreePop();
			}
			if (ImGui::IsItemClicked() && !_clicked)
			{
				_selectedGameObject = go;
				_clicked = true;
			}
		}
	}


	if (ImGui::IsWindowHovered()) 
	{
		if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
		{
			ImGui::OpenPopup("Menu");
		}
	}
	if (ImGui::BeginPopup("Menu"))
	{
		if (ImGui::Selectable((char*)u8"오브젝트 생성"))
		{
			GameObject::Instantiate(L"GameObject");
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

void TestDemo::ShowInspector()
{
	ImGui::Begin("Inspector");
	if (_selectedGameObject)
	{
		// 이름
		wstring text = _selectedGameObject->GetName();
		char name[256];
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, name, 256, NULL, NULL);

		ImGui::Text(name);

		// Transfrom
		{
			if (ImGui::TreeNode("Transfrom"))
			{
				shared_ptr<Transform> tr = _selectedGameObject->GetCompoent<Transform>();
				Vector3 pos = tr->GetLocalPosition();
				if (ImGui::InputFloat3("Position", (float*)&pos))
				{
					tr->SetLocalPosition(pos);
					auto rigid = _selectedGameObject->GetCompoent<RigidBody>();
					if (rigid != nullptr)
					{
						rigid->SetPosition(pos);
						rigid->SetVelocity(Vector3::Zero);
					}
				}
				Vector3 scale = tr->GetLocalScale();
				if (ImGui::InputFloat3("Scale", (float*)&scale))
					tr->SetLocalScale(scale);
				Vector3 rotation = tr->GetLocalRotation();
				if (ImGui::InputFloat3("Rotation", (float*)&rotation))
					tr->SetLocalRotation(rotation);
				ImGui::TreePop();
			}
		}

		// Animator
		{
			shared_ptr<Animator> animator = _selectedGameObject->GetCompoent<Animator>();

			if (animator != nullptr) {
				shared_ptr<Model> model = animator->GetModel();

				if (ImGui::TreeNode("Animator"))
				{
					bool isPlay = animator->GetIsPlay();
					if (ImGui::Checkbox("Play", &isPlay))
					{
						if (isPlay) animator->Play();
						else animator->Stop();
					}

					const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)animator.get());
					ImGui::BeginChild(child_id, ImVec2(100, 200.0f), ImGuiChildFlags_::ImGuiChildFlags_Border);

					if (model != nullptr) {
						for (int i = 0; i < model->GetAnimationCount(); i++)
						{
							shared_ptr<AnimationClip> clip = model->GetAnimationByIndex(i);
							wstring text = clip->GetAnimationDesc().name;
							char name[256];
							WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, name, 256, NULL, NULL);

							if (ImGui::Selectable(name))
							{
								animator->SetNextAnimationClip(i);
							}
						}
					}

					ImGui::EndChild();
					ImGui::TreePop();
				}
			}
		}

		// SkyBox
		shared_ptr<SkyBox> skyBox = _selectedGameObject->GetCompoent<SkyBox>();
		if (skyBox) {
			if (ImGui::TreeNode("SkyBox"))
			{
				auto material = skyBox->GetMaterial();
				if (material == nullptr)
				{
					if (ImGui::Button("Material", ImVec2(50, 50)))
						OpenMaterialSelection();
				}
				else {
					if (ImGui::ImageButton(material->GetDiffuseMap()->GetResourceView().Get(), ImVec2(50, 50)))
						OpenMaterialSelection();
				}
				material = ShowMaterialSelection();
				if (material != nullptr)
					skyBox->SetMaterial(material);

				ImGui::TreePop();
			}
		}

		// MeshRenderer
		shared_ptr<MeshRenderer> meshRenderer = _selectedGameObject->GetCompoent<MeshRenderer>();
		if (meshRenderer)
		{
			if (ImGui::TreeNode("MeshRenderer"))
			{
				auto mesh = meshRenderer->GetMesh();
				if (mesh == nullptr)
				{
					if (ImGui::Selectable("NoMesh"))
					{
						OpenMeshSelection();
					}
				}
				else
				{
					if (ImGui::Selectable(Utils::ToUtf8(mesh->GetName()).c_str()))
					{
						OpenMeshSelection();
					}
				}
				mesh = ShowMeshSelection();
				if (mesh != nullptr)
					meshRenderer->SetMesh(mesh);



				auto material = meshRenderer->GetMaterial();
				if (material == nullptr)
				{
					if (ImGui::Button("Material", ImVec2(50, 50)))
						OpenMaterialSelection();
				}
				else {
					if (ImGui::ImageButton(material->GetDiffuseMap()->GetResourceView().Get(), ImVec2(50, 50)))
						OpenMaterialSelection();
				}

				material = ShowMaterialSelection();
				if (material != nullptr)
					meshRenderer->SetMaterial(material);

				ImGui::TreePop();
			}
		}

		// RigidBody
		shared_ptr<RigidBody> rigidBody = _selectedGameObject->GetCompoent<RigidBody>();
		if (rigidBody)
		{
			if (ImGui::TreeNode("RigidBody"))
			{
				ImGui::TreePop();
			}
		}

		// BoxCollider
		shared_ptr<BoxCollider> boxCollider = _selectedGameObject->GetCompoent<BoxCollider>();
		if (boxCollider)
		{
			if (ImGui::TreeNode("BoxCollider"))
			{
				Vector3 scale = boxCollider->GetScale();
				Vector3 offset = boxCollider->GetOffset();
				if (ImGui::DragFloat3("Offset", (float*)&offset, 0.01f))
				{
					boxCollider->SetOffset(offset);
				}
				if (ImGui::DragFloat3("Scale", (float*)&scale, 0.01f))
				{
					boxCollider->SetScale(scale);
				}

				ImGui::TreePop();
			}
		}
		// Light
		shared_ptr<Light> light = _selectedGameObject->GetCompoent<Light>();
		if (light)
		{
			if (ImGui::TreeNode("Light"))
			{
				const LightDesc& desc = light->GetLightDesc();
				Color diffuse = desc.diffuse;
				if (ImGui::DragFloat4("Diffuse", (float*)&diffuse, 0.01f, 0.0f, 1.0f))
				{
					light->SetDiffuse(diffuse);
				}
				Color ambient = desc.ambient;
				if (ImGui::DragFloat4("Ambient", (float*)&ambient, 0.01f, 0.0f, 1.0f))
				{
					light->SetAmbient(ambient);
				}
				Color specular = desc.specular;
				if (ImGui::DragFloat4("Specular", (float*)&specular, 0.01f, 0.0f, 1.0f))
				{
					light->SetSpecular(specular);
				}
				Color emissive = desc.emissve;
				if (ImGui::DragFloat4("Emissve", (float*)&emissive, 0.01f, 0.0f, 1.0f))
				{
					light->SetEmissive(emissive);
				}
				Vector3	 direction = desc.direction;
				if (ImGui::DragFloat3("Direction", (float*)&direction, 0.01f, -1.0f, 1.0f)) {
					light->SetDIrection(direction);
				}

				if (_shadowMap != nullptr && _shadowMap->DepthMapSRV() != nullptr)
					ImGui::Image(_shadowMap->DepthMapSRV().Get(), ImVec2(50, 50));


				ImGui::TreePop();
			}
		}

		// Terrain
		shared_ptr<Terrain> terrain = _selectedGameObject->GetCompoent<Terrain>();
		if (terrain)
		{
			if (ImGui::TreeNode("Terrain"))
			{
				Vector2 size = _terrainDesc->size;
				float cellSpacing = _terrainDesc->cellSpacing;

				if (ImGui::InputFloat2("Size ", (float*)&size))
				{
					_terrainDesc->size = size;
				}
				if (ImGui::InputFloat("Spacing ", &cellSpacing))
				{
					_terrainDesc->cellSpacing = cellSpacing;
				}



				ImGui::BeginTable("split", 4);

				float textureSize = ImGui::CalcItemWidth() / 4;
				for (int i = 0; i < 4; i++) {
					ImGui::TableNextColumn();
					auto material = _terrainDesc->materials[i];
					if (material == nullptr)
					{
						if (ImGui::Button("Material", ImVec2(textureSize, textureSize))) {
							OpenMaterialSelection();
							_terrainSelectIndex = i;
						}
					}
					else {
						if (ImGui::ImageButton(material->GetDiffuseMap()->GetResourceView().Get(), ImVec2(textureSize, textureSize))) {
							OpenMaterialSelection();
							_terrainSelectIndex = i;
						}
					}
				}
				auto material = ShowMaterialSelection();
				if (material != nullptr)
					_terrainDesc->materials[_terrainSelectIndex] = material;
				ImGui::EndTable();

				if (ImGui::Button("Open Editor"))
				{
					_terrainPicker->SetTerrain(terrain);
					_selectedTerrain = terrain;
				}

				if (ImGui::Button("Create"))
				{
					terrain->Create(*_terrainDesc);
				}

				ImGui::TreePop();
			}
		}

		if (_selectedTerrain)
		{
			ShowTerrainEditor();
		}


		// TerrainCollider
		shared_ptr<TerrainCollider> terrainCollider = _selectedGameObject->GetCompoent<TerrainCollider>();
		if (terrainCollider)
		{
			if (ImGui::TreeNode("TerrainCollider"))
			{
				ImGui::TreePop();
			}
		}


		// AddComponent
		if (ImGui::Button("AddComponent"))
		{
			ImGui::OpenPopup("Component");
		}
		if (ImGui::BeginPopup("Component"))
		{
			for (int i = 0; i < FIXED_COMPONENT_COUNT; i++)
			{
				if (ImGui::Selectable(ComponentNames[i].c_str()))
				{
					_selectedGameObject->AddComponent(static_cast<ComponentType>(i));
				}
			}
			ImGui::EndPopup();
		}

	}
	ImGui::End();
}
shared_ptr<GameObject> CreateMeshGameObject(shared_ptr<Mesh> _mesh)
{
	auto gameObject = GameObject::Instantiate(L"Mesh");

	auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
	meshRenderer->SetMesh(_mesh);
	meshRenderer->SetMaterial(RESOURCE->Get<Material>(L"Default"));

	return gameObject;

}
void TestDemo::ShowGameObjectCreator()
{
	ImGui::Begin("Shape");
	const map<wstring, shared_ptr<ResourceBase>>& meshes = RESOURCE->GetResources<Mesh>();

	for (auto mesh : meshes)
	{
		ImGui::Selectable(Utils::ToString(mesh.first).c_str());
		if (ImGui::BeginDragDropSource())
		{
			if (_generatedGameObject == nullptr)
			{
				_generatedGameObject = CreateMeshGameObject(static_pointer_cast<Mesh>(mesh.second));
			}
			ImGui::EndDragDropSource();
		}

	}

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
	{
		if (_generatedGameObject != nullptr)
		{
		}
	}
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		if (_generatedGameObject != nullptr)
		{
			POINT mousePos = INPUT->GetMousePos();
			Vector3 pos = _camera->GetCompoent<Camera>()->ScreenToWorldPosition(Vector4(mousePos.x, mousePos.y, 1, 1));
			_generatedGameObject->GetTransform()->SetPosition(pos + _camera->GetTransform()->GetLook() * 10);
		}

	}
	if (INPUT->GetButtonUp(KEY_TYPE::LBUTTON))
	{
		if (_generatedGameObject != nullptr)
		{
			_generatedGameObject = nullptr;
		}
	}

	ImGui::End();
}
shared_ptr<GameObject> CreateModel(shared_ptr<Model> model)
{
	auto gameObject = GameObject::Instantiate(L"Model");

	auto bones = model->Instantiate();
	if (bones.size() >= 1)
		bones[0]->GetTransform()->SetParent(gameObject->GetTransform());



	return gameObject;

}

void TestDemo::ShowModels()
{
	ImGui::Begin("Models");
	const map<wstring, shared_ptr<ResourceBase>>& models = RESOURCE->GetResources<Model>();

	for (auto model : models)
	{
		ImGui::Selectable(Utils::ToString(model.first).c_str());
		if (ImGui::BeginDragDropSource())
		{
			if (_generatedGameObject == nullptr)
			{
				_generatedGameObject = CreateModel(static_pointer_cast<Model>(model.second));
			}
			ImGui::EndDragDropSource();
		}

	}
	if (INPUT->GetButton(KEY_TYPE::LBUTTON))
	{
		if (_generatedGameObject != nullptr)
		{
			POINT mousePos = INPUT->GetMousePos();
			Vector3 pos = _camera->GetCompoent<Camera>()->ScreenToWorldPosition(Vector4(mousePos.x, mousePos.y, 1, 1));
			_generatedGameObject->GetTransform()->SetPosition(pos + _camera->GetTransform()->GetLook() * 10);
		}

	}
	if (INPUT->GetButtonUp(KEY_TYPE::LBUTTON))
	{
		if (_generatedGameObject != nullptr)
		{
			_generatedGameObject = nullptr;
		}
	}

	ImGui::End();
}

void TestDemo::ShowTerrainEditor()
{
	ImGui::Begin("TerrainEditor");
	if (ImGui::BeginTable("split", 6))
	{
		ImGui::TableNextColumn();
		if (ImGui::Button((char*)u8"높이", ImVec2(50, 50)))
		{
			_paintType = PaintType::Height;
		}
		if (ImGui::Button((char*)u8"부드럽게", ImVec2(50, 50)))
		{
			_paintType = PaintType::Smooth;
		}

		for (int i = 0; i < 4; i++) {
			ImGui::TableNextColumn();
			auto view = _selectedTerrain->GetTextureSRV(i);
			if (view != nullptr) {
				if (ImGui::ImageButton(view.Get(), ImVec2(50, 50)))
				{
					_paintType = PaintType::Texture;
					_paintIndex = i;
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::SliderFloat((char*)u8"브러쉬크기", &_brushSize, 0.1f, 20.0f);
	ImGui::SliderFloat((char*)u8"강도", &_brushIntensity, 0.1f, 20.0f);

	ImGui::InputText((char*)u8"파일이름", _terrainFileName, sizeof(_terrainFileName));
	if (ImGui::Button((char*)u8"저장"))
	{
		string str(_terrainFileName);
		_selectedTerrain->SaveMapData(str);
	}
	if (ImGui::Button((char*)u8"불러오기"))
	{
		string str(_terrainFileName);
		_selectedTerrain->LoadMapData(str);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		_selectedTerrain->SetPass(0);
	if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		_selectedTerrain->SetPass(1);

	if (!ImGui::IsWindowHovered())
	{
		if (INPUT->GetButton(KEY_TYPE::LBUTTON))
		{
			const Vector3& mousePos = _terrainPicker->GetMousePositionOnTerrain(INPUT->GetMousePos().x, INPUT->GetMousePos().y);
			_terrainPicker->Pick(mousePos);
			for (auto index : _terrainPicker->_pickingIndices)
			{
				if (_paintType == PaintType::Height)
				{
					_selectedTerrain->ModifyHeight(index, INPUT->GetButton(KEY_TYPE::LSHIFT) ? -_brushIntensity : _brushIntensity, _terrainPicker->_point, _brushSize);
				}
				if (_paintType == PaintType::Smooth)
				{
					_selectedTerrain->SmoothHeight(index, _terrainPicker->_point, _brushSize);
				}
				else if (_paintType == PaintType::Texture)
				{
					_selectedTerrain->ModifyBlend(index, _paintIndex, _terrainPicker->_point, _brushSize);
				}
			}
		}
	}
	ImGui::End();
}

