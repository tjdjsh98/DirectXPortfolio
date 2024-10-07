#include "pch.h"
#include "Gizmo.h"
#include "Shader.h"
#include "InstancingBuffer.h"
#include "Camera.h"

void Gizmo::DrawLine(Vector3 start, Vector3 end,Color color	)
{
	Vector3 pos = start;
	Vector3 scale = end - start;

	Matrix S = Matrix::CreateScale(scale);
	Matrix P = Matrix::CreateTranslation(pos);

	_lineMatrix.push_back(S * P);
	_lineColors.push_back(color);

}

void Gizmo::DrawCube(Vector3 pos,Vector3 scale, Color color)
{
	_cubePositions.push_back(pos);
	_cubeScales.push_back(scale);
	_cubeColors.push_back(color);
}

void Gizmo::Draw()
{
	DrawCube();
	DrawLine();
}

void Gizmo::DrawCube()
{
	if (_cubePositions.size() <= 0)return;
	if (_cubeMesh == nullptr)
	{
		_instancingBuffer = make_shared<InstancingBuffer>();

		_cubeMesh = RESOURCE->Get<Mesh>(L"Cube");
		_shader = RESOURCE->Get<Shader>(L"Default");
	}

	_instancingBuffer->ClearData();

	for(int i = 0 ; i< _cubePositions.size(); i++)
	{
		InstancingData instancingData;
		{
			Matrix matrix = Matrix::CreateScale(_cubeScales[i]);
			matrix *= Matrix::CreateTranslation(_cubePositions[i]);
			instancingData.world = matrix;
			instancingData.color = _cubeColors[i];
		}

		_instancingBuffer->AddData(instancingData);
	}

	//GlobalBuffer
	GlobalDesc global = {};
	global.V = Camera::S_MatView;
	global.P = Camera::S_MatProjection;
	global.VP = global.V * global.P;
	global.VInv = global.V.Invert();
	_shader->PushGlobalData(global);


	_cubeMesh->GetVertexBuffer()->PushData();
	_cubeMesh->GetIndexBuffer()->PushData();
	_instancingBuffer->PushData();

	_shader->DrawIndexedInstanced(L"GizmoTech", 0, _cubeMesh->GetIndexBuffer()->GetCount(), _instancingBuffer->GetCount());
	_cubePositions.clear();
	_cubeScales.clear();
	_cubeColors.clear();
}

void Gizmo::DrawLine()
{
	if (_lineMatrix.size() <= 0)return;
	if (_lineMesh == nullptr)
	{
		_lineMesh = RESOURCE->Get<Mesh>(L"Line");
		_shader = RESOURCE->Get<Shader>(L"Default");
	}
	if (_instancingBuffer == nullptr)
		_instancingBuffer = make_shared<InstancingBuffer>();

	_instancingBuffer->ClearData();
	for(int i = 0 ; i < _lineMatrix.size() ; i++)
	{
		InstancingData instancingData;
		{
			instancingData.world = _lineMatrix[i];
			instancingData.color = _lineColors[i];
		}

		_instancingBuffer->AddData(instancingData);
	}

	//GlobalBuffer
	GlobalDesc global = {};
	global.V = Camera::S_MatView;
	global.P = Camera::S_MatProjection;
	global.VP = global.V * global.P;
	global.VInv = global.V.Invert();
	_shader->PushGlobalData(global);
	
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	_lineMesh->GetVertexBuffer()->PushData();
	_lineMesh->GetIndexBuffer()->PushData();
	_instancingBuffer->PushData();

	_shader->DrawIndexedInstanced(L"GizmoTech", 0, _cubeMesh->GetIndexBuffer()->GetCount(), _instancingBuffer->GetCount());

	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_lineMatrix.clear();
	_lineColors.clear();
}
