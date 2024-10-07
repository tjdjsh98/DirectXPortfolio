#include "pch.h"
#include "ResourceManager.h"
#include <filesystem>
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "Model.h"

void ResourceManager::Init()
{
	CreateDefaultMesh();
	CreateDefaultShader();
	{
		auto model = make_shared<Model>();
		model->ReadModel(L"TelephoneBooth/TelephoneBooth");
		model->ReadMaterial(L"TelephoneBooth/TelephoneBooth");
		Add(L"TelephoneBooth", model);
	}
	{
		auto model = make_shared<Model>();
		model->ReadModel(L"Kachujin/Kachujin");
		model->ReadMaterial(L"Kachujin/Kachujin");
		model->ReadAnimation(L"Kachujin/Idle");
		model->ReadAnimation(L"Kachujin/Run");
		model->ReadAnimation(L"Kachujin/Slash");
		Add(L"Kachujin", model);
	}

	Load<Texture>(L"Grass_Color", L"..\\Resources\\Textures\\Terrain\\Grass01\\Grass01_1K_BaseColor.png");
	Load<Texture>(L"Grass_Normal", L"..\\Resources\\Textures\\Terrain\\Grass01\\Grass01_1K_Normal.png");
	Load<Texture>(L"Grass_Height", L"..\\Resources\\Textures\\Terrain\\Grass01\\Grass01_1K_Height.png");

	Load<Texture>(L"Moss_Color", L"..\\Resources\\Textures\\Terrain\\Moss01\\Moss01_1K_BaseColor.png");
	Load<Texture>(L"Moss_Normal", L"..\\Resources\\Textures\\Terrain\\Moss01\\Moss01_1K_Normal.png");
	Load<Texture>(L"Moss_Height", L"..\\Resources\\Textures\\Terrain\\Moss01\\Moss01_1K_Height.png");

	Load<Texture>(L"Rock_Color", L"..\\Resources\\Textures\\Terrain\\Rock07\\Rock07_1K_BaseColor.png");
	Load<Texture>(L"Rock_Normal", L"..\\Resources\\Textures\\Terrain\\Rock07\\Rock07_1K_Normal.png");
	Load<Texture>(L"Rock_Height", L"..\\Resources\\Textures\\Terrain\\Rock07\\Rock07_1K_Height.png");

	Load<Texture>(L"SandyGravel_Color", L"..\\Resources\\Textures\\Terrain\\SandyGravel02\\SandyGravel02_1K_BaseColor.png");
	Load<Texture>(L"SandyGravel_Normal", L"..\\Resources\\Textures\\Terrain\\SandyGravel02\\SandyGravel02_1K_Normal.png");
	Load<Texture>(L"SandyGravel_Height", L"..\\Resources\\Textures\\Terrain\\SandyGravel02\\SandyGravel02_1K_Height.png");

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetDiffuseMap(RESOURCE->Get<Texture>(L"Grass_Color"));
		material->SetNormalMap(RESOURCE->Get<Texture>(L"Grass_Normal"));
		material->SetHeightMap(RESOURCE->Get<Texture>(L"Grass_Height"));
		Add(L"Grass", material);
	}
	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetDiffuseMap(RESOURCE->Get<Texture>(L"Moss_Color"));
		material->SetNormalMap(RESOURCE->Get<Texture>(L"Moss_Normal"));
		material->SetHeightMap(RESOURCE->Get<Texture>(L"Moss_Height"));
		Add(L"Moss", material);
	}
	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetDiffuseMap(RESOURCE->Get<Texture>(L"Rock_Color"));
		material->SetNormalMap(RESOURCE->Get<Texture>(L"Rock_Normal"));
		material->SetHeightMap(RESOURCE->Get<Texture>(L"Rock_Height"));
		Add(L"Rock", material);
	}
	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetDiffuseMap(RESOURCE->Get<Texture>(L"SandyGravel_Color"));
		material->SetNormalMap(RESOURCE->Get<Texture>(L"SandyGravel_Normal"));
		material->SetHeightMap(RESOURCE->Get<Texture>(L"SandyGravel_Height"));
		Add(L"SandyGravel", material);
	}
	{

		
		{
			shared_ptr<Material> material = make_shared<Material>();
			material->SetDiffuseMap(RESOURCE->Load<Texture>(L"Default", L"..\\Resources\\Texture\\1.jpg"));
			material->SetShader(Get<Shader>(L"Default"));
			Add(L"Default", material);

		}
		{
			shared_ptr<Material> material = make_shared<Material>();
			material->SetDiffuseMap(RESOURCE->Load<Texture>(L"Sky", L"..\\Resources\\Textures\\Sky01.jpg"));
			material->SetShader(Get<Shader>(L"Default"));
			Add(L"Sky", material);
		}
	}
}



void ResourceManager::CreateDefaultMesh()
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetName(L"Quad");
	mesh->CreateQuad();
	Add(L"Quad", mesh);

	mesh = make_shared<Mesh>();
	mesh->SetName(L"Cube");
	mesh->CreateCube();
	Add(L"Cube", mesh);

	mesh = make_shared<Mesh>();
	mesh->SetName(L"Sphere");
	mesh->CreateSphere();
	Add(L"Sphere", mesh);

	mesh = make_shared<Mesh>();
	mesh->SetName(L"Line");
	mesh->CreateLine();
	Add(L"Line", mesh);

}

void ResourceManager::CreateDefaultShader()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"Default.fx");
	Add(L"Default", shader);

	shader = make_shared<Shader>(L"Terrain.fx");
	Add(L"Terrain", shader);
}
