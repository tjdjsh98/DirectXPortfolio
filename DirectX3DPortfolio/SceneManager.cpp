#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

void SceneManager::Init()
{
	ChangeScene(CreateScene());
}

void SceneManager::Update()
{
	if (_currentScene == nullptr)
		return;

	_currentScene->Update();
	_currentScene->LateUpdate();
	_currentScene->Render();
}

shared_ptr<Scene> SceneManager::CreateScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();

	return scene;
}
