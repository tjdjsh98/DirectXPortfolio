#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "tinyxml2.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"
#include "ResourceManager.h"

Material::Material() : ResourceBase(ResourceType::Material)
{
	
}

Material::~Material()
{
	
}

void Material::SetShader(shared_ptr<Shader> shader)
{
	_shader = shader;
	
	_diffuseEffectBuffer = _shader->GetSRV("DiffuseMap");
	_normalEffectBuffer = _shader->GetSRV("NormalMap");
	_specularEffectBuffer = _shader->GetSRV("SpecularMap");
}

void Material::Update()
{
	if (_shader == nullptr)
		return;

	_shader->PushMaterialData(_desc);

	if (_diffuseMap) 
		_diffuseEffectBuffer->SetResource(_diffuseMap->GetResourceView().Get());

	if (_normalMap)
		_normalEffectBuffer->SetResource(_normalMap->GetResourceView().Get());

	if (_specularMap)
		_specularEffectBuffer->SetResource(_specularMap->GetResourceView().Get());
}

void Material::Load(const wstring& path)
{
	wstring fullPath = path + L".xml";
	auto parentPath = filesystem::path(fullPath).parent_path();

	tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = document->FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{

		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		SetName(Utils::ToWString(node->GetText()));

		// Diffuse Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
				SetDiffuseMap(texture);
			}
		}

		// Specular Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring texture = Utils::ToWString(node->GetText());
			if (texture.length() > 0)
			{
				wstring textureStr = Utils::ToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
					SetSpecularMap(texture);
				}
			}
		}

		// Normal Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				shared_ptr<Texture> texture = RESOURCE->Load<Texture>(textureStr, (parentPath / textureStr).wstring());
				SetNormalMap(texture);
			}
		}

		// Ambient
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			GetMaterialDesc().ambient = color;
		}

		// Diffuse
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			GetMaterialDesc().diffuse = color;
		}

		// Specular
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			GetMaterialDesc().specular = color;
		}

		// Emissive
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			GetMaterialDesc().emissive = color;
		}
	}
}
