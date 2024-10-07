#include "pch.h"
#include "EditorTool.h"
#include "CommonFileDialogApp.h"
#include "Converter.h"
#include <sstream>
#include <ResourceManager.h>
#include <iostream>
#include <Utils.h>

EditorTool::EditorTool()
{
	
}

EditorTool::~EditorTool()
{
}
void EditorTool::Init()
{
	_fileDialog = make_shared<CommonFileDialogApp>();
	_converter = make_shared<Converter>();
}

void EditorTool::Awake()
{
}

vector<wstring> split(wstring input,wchar_t delimiter)
{
	vector<wstring> answer;
	std::wstringstream ss(input);
	wstring temp;

	while (std::getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

void EditorTool::Update()
{
	UpdateGUI();
}

void EditorTool::UpdateGUI()
{
	static float a = 1;
	static int counter = 0;
	ImGui::Begin((char*)u8"한글");

	ImGui::SliderFloat("float", &a, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::Text("%f %f %f", GAME->GetGameDesc().clearColor.x, GAME->GetGameDesc().clearColor.y, GAME->GetGameDesc().clearColor.z);

	if (ImGui::Button("Read Asset"))
	{
		_fileDialog->AddItemsToCommonPlaces();
		FileType fileType = _fileDialog->GetFileType();
		if (_fileDialog->GetPath() != nullptr) {
			const wstring& filePath = _fileDialog->GetPath();

			if (fileType == FileType::Fbx)
			{
				_converter->ReadAssetFile(filePath);
			}
		}
	}
	if (_fileDialog->GetPath() == nullptr)
	{
		ImGui::Text((char*)u8"파일을 선택.");
	}
	else
	{
		const wstring& filePath = _fileDialog->GetPath();
		string path;
		path.assign(filePath.begin(), filePath.end());
		ImGui::Text((char*)(path.c_str()));
		if (ImGui::Button("Export Model"))
		{
			vector<wstring> parts = split(filePath, L'\\');
			_converter->ExportModelData(parts.at(parts.size() - 3) + L"\\" + parts.at(parts.size() - 3));
		}
		if (ImGui::Button("Export Texture"))
		{
			vector<wstring> parts = split(filePath, L'\\');

			_converter->ExportMaterialData(parts.at(parts.size() - 3) + L"\\" + parts.at(parts.size() - 3));
		}

		if (ImGui::Button("Export Animation"))
		{
			vector<wstring> parts = split(filePath, L'\\');
			_converter->ExportAnimationData(parts.at(parts.size() - 2) + L"/" + split(parts.at(parts.size() - 1), L'.')[0]);
		}
	}

	ImGui::End();


	auto& resourceArray = RESOURCE->GetResources(); 

	ImGui::Begin((char*)u8"리소스");

	for (int i = 0; i < RESOURCE_TYPE_COUNT; i++)
	{
		if (resourceArray[i].size() == 0) continue;
			string resourceName = "";
		switch (i)
		{
		case 0:
			resourceName = "Mesh";
			break;
		case 1:
			resourceName = "Shader";
			break;
		case 2:
			resourceName = "Texture";
			break;
		case 3:
			resourceName = "Material";
			break;
		case 4:
			resourceName = "Animation";
			break;
		default:
			resourceName = "None";
			break;
		}

		if (ImGui::TreeNode(resourceName.c_str()))
		{
			for (auto pair : resourceArray[i])
			{
				ImGui::Text(Utils::ToString(pair.first).c_str());
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}



