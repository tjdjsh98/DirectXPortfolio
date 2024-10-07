#pragma once

class MeshRenderer;
class Mesh;
class Material;
class GameObject;
class Shader;
class Transform;
class Texture;
class Model;
class Terrain;
class SkinnedMeshRenderer;
class TerrainPicker;
class ShadowMap;
struct TerrainDesc;

enum class PaintType
{
	Height,
	Smooth,
	Texture,
};

class TestDemo : public IExcute
{
public:
	TestDemo();
	virtual ~TestDemo() override;

	// IExcute��(��) ���� ��ӵ�
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	void DisplayNode(shared_ptr<GameObject> parent);

	void OpenMeshSelection();
	shared_ptr<Mesh> ShowMeshSelection();

	void OpenMaterialSelection();
	shared_ptr<Material> ShowMaterialSelection();

	void ShowHierarchy();
	void ShowInspector();
	void ShowGameObjectCreator();
	void ShowModels();

	void ShowTerrainEditor();

private:
	shared_ptr<GameObject> _character = nullptr;
	shared_ptr<Model> _model = nullptr;
	shared_ptr<Shader> _shader = nullptr;
	shared_ptr<TerrainPicker> _picker = nullptr;

	shared_ptr<GameObject> _camera;
	shared_ptr<GameObject> _modelMesh;
	shared_ptr<Terrain> _terrain;
	shared_ptr<TerrainPicker> _terrainPicker;

	shared_ptr<ShadowMap> _shadowMap;

	vector<shared_ptr<SkinnedMeshRenderer>> _characterMesh;

	// ���̶�Ű
	vector<shared_ptr<GameObject>> _hierarchyGameObjects;
	shared_ptr<GameObject> _selectedGameObject;
	bool _clicked;

	// �ν�����
	bool _openMaterialSelection;
	TerrainDesc* _terrainDesc;
	int _terrainSelectIndex;


	// �׷��� ������
	shared_ptr<Terrain> _selectedTerrain;
	PaintType _paintType;
	uint8 _paintIndex = 0;
	float _brushSize = 0;
	float _brushIntensity = 0;
	char _terrainFileName[128]{ 0, };
	// �޽� ������ ���
	shared_ptr<GameObject> _generatedGameObject;
};
