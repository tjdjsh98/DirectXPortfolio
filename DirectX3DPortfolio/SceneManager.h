#pragma once

class Scene;

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Init();
	void Update();

	shared_ptr<Scene> CreateScene();
	template<typename T>
	void ChangeScene(shared_ptr<T> scene)
	{
		_currentScene = scene;
		scene->Start();
	}

	shared_ptr<Scene> GetCurrentScene() { return _currentScene; }

private:
	shared_ptr<Scene> _currentScene = make_shared<Scene>();
};

