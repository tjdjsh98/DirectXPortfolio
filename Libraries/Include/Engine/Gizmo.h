#pragma once

class Gizmo
{
	DECLARE_SINGLE(Gizmo);

public:
	void DrawLine(Vector3 start, Vector3 end, Color color = Color{ 0,0,0,0 });
	void DrawCube(Vector3 pos,Vector3 scale, Color color = Color{ 0,0,0,0 });


private:
	friend Game;
	void Draw();
	void DrawCube();
	void DrawLine();

private:
	shared_ptr<Shader> _shader;
	shared_ptr<InstancingBuffer> _instancingBuffer;

	shared_ptr<Mesh> _cubeMesh;
	shared_ptr<Mesh> _lineMesh;

	vector<Vector3> _cubePositions;
	vector<Vector3> _cubeScales;
	vector<Color> _cubeColors;
	vector<Matrix> _lineMatrix;
	vector<Color> _lineColors;
};

