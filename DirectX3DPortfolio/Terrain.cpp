#include "pch.h"
#include "Terrain.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "TerrainCollider.h"
#include "GeometryHelper.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include <filesystem>


Terrain::Terrain() : Component(ComponentType::Terrain)
{
}

Terrain::~Terrain()
{
}

void Terrain::Create(const TerrainDesc& terrainDesc)
{
	_terrainDesc = terrainDesc;

	_numPatchVertexCols = (_terrainDesc.size.x - 1) / CellsPerPatch + 1;
	_numPatchVertexRows = (_terrainDesc.size.y - 1) / CellsPerPatch + 1;

	_numPatchVertices = _numPatchVertexCols * _numPatchVertexRows;
	_numPatchQuadFaces = (_numPatchVertexCols - 1) * (_numPatchVertexRows - 1);

	
	CreateTextureSRV();

	_material = make_shared<Material>();
	auto shader = RESOURCE->Get<Shader>(L"Default");
	_material->SetShader(shader);

	if (terrainDesc.loadDataName != "")
	{
		LoadMapData(terrainDesc.loadDataName);
	}

	// Material
	MaterialDesc desc;
	{
		desc.ambient = Color(1, 1, 1, 1);
		desc.diffuse = { 1,1,1,1 };
		desc.emissive = { 0,0,0,0 };
		desc.specular = Color(0.2f, 0.2f, 0.2f, 0.2f);
	}
	_material->SetMaterialDesc(desc);
	
	CreateRawVertices();
	CreateRawIndices();

	BuildQuadPatchVB();
	BuildQuadPatchIB();

	//CreateRawVertices();
	//_rawVertexBuffer = make_shared<VertexBuffer>();
	//_rawVertexBuffer->Create(_vertices, 0);

	//CreateRawIndices();
	//_rawIndexBuffer = make_shared<IndexBuffer>();
	//_rawIndexBuffer->Create(_indices);

	_numPatchVertexRows = ((_terrainDesc.size.y - 1) / CellsPerPatch) + 1;
	_numPatchVertexCols = ((_terrainDesc.size.x - 1) / CellsPerPatch) + 1;

	_numPatchVertices = _numPatchVertexCols * _numPatchVertexRows;
	_numPatchQuadFaces = (_numPatchVertexRows - 1) * (_numPatchVertexCols - 1);

	_heightMap.clear();
	_heightMap.resize((_terrainDesc.size.x) * (_terrainDesc.size.y),0.0f);
	BuildHeightSRV();
	_blendMap.clear();
	_blendMap.resize((_terrainDesc.size.x) * (_terrainDesc.size.y), Vector4(0,0,0,0));
	BuildBlendSRV();

	
}

void Terrain::CreateRawVertices()
{
	_rawVertices.resize((_terrainDesc.size.x +1) * (_terrainDesc.size.y +1));
	float halfWidth = ((_terrainDesc.size.x -1) * _terrainDesc.cellSpacing) / 2.0f;
	float halfDepth = ((_terrainDesc.size.y -1) * _terrainDesc.cellSpacing) / 2.0f;
	
	for (uint32 z = 0; z < _terrainDesc.size.y +1; z++)
	{
		float vz = -halfDepth + (z* _terrainDesc.cellSpacing);
		for (uint32 x = 0; x < _terrainDesc.size.x +1; x++)
		{
			float vx = -halfWidth + (x * _terrainDesc.cellSpacing);

			_rawVertices[z * (_terrainDesc.size.x +1) + x] = Vector3(vx, 0, vz);
		}
	}

}

void Terrain::CreateRawIndices()
{
	_rawIndices.clear();
	for (int32 z = 0; z < _terrainDesc.size.y; z++)
	{
		for (int32 x = 0; x < _terrainDesc.size.x ; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z + 1) + (x));
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z)+(x + 1));
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z)+(x));
			//  [1] - [2]
			//   	\  |
			//		  [0]
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z)+(x + 1));
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z + 1) + (x));
			_rawIndices.push_back((_terrainDesc.size.x +1) * (z + 1) + (x + 1));
		}
	}
}

void Terrain::CreateTextureSRV()
{
	// Diffuse
	{
		D3D11_TEXTURE2D_DESC texElementDesc;
		_terrainDesc.materials[0]->GetDiffuseMap()->GetTexture()->GetDesc(&texElementDesc);


		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = _terrainDesc.materials.size();
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> texArray;
		DEVICE->CreateTexture2D(&texArrayDesc, 0, texArray.GetAddressOf());

		for (uint32 texElement = 0; texElement < _terrainDesc.materials.size(); ++texElement)
		{
			// for each mipmap level...
			for (uint32 mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
			{
				if (_terrainDesc.materials[texElement] == nullptr) continue;
				if (_terrainDesc.materials[texElement]->GetDiffuseMap() == nullptr) continue;

				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				ComPtr<ID3D11Texture2D> texture = _terrainDesc.materials[texElement]->GetDiffuseMap()->GetTexture();
				HRESULT hr =DC->Map(texture.Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
				CHECK(hr);

				DC->UpdateSubresource(texArray.Get(),
					::D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
					0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

				DC->Unmap(_terrainDesc.materials[texElement]->GetDiffuseMap()->GetTexture().Get(), mipLevel);
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = _terrainDesc.materials.size();

		CHECK(DEVICE->CreateShaderResourceView(texArray.Get(), &viewDesc, _diffuseArraySrv.GetAddressOf()));
	}

	// Normal
	{
		D3D11_TEXTURE2D_DESC texElementDesc;
		_terrainDesc.materials[0]->GetNormalMap()->GetTexture()->GetDesc(&texElementDesc);


		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = _terrainDesc.materials.size();
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> texArray;
		DEVICE->CreateTexture2D(&texArrayDesc, 0, texArray.GetAddressOf());

		for (uint32 texElement = 0; texElement < _terrainDesc.materials.size(); ++texElement)
		{
			// for each mipmap level...
			for (uint32 mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
			{
				if (_terrainDesc.materials[texElement] == nullptr) continue;
				if (_terrainDesc.materials[texElement]->GetNormalMap() == nullptr) continue;

				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				CHECK(DC->Map(_terrainDesc.materials[texElement]->GetNormalMap()->GetTexture().Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

				DC->UpdateSubresource(texArray.Get(),
					::D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
					0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

				DC->Unmap(_terrainDesc.materials[texElement]->GetNormalMap()->GetTexture().Get(), mipLevel);
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = _terrainDesc.materials.size();

		CHECK(DEVICE->CreateShaderResourceView(texArray.Get(), &viewDesc, _normalArraySrv.GetAddressOf()));
	}

	// Height
	{
		D3D11_TEXTURE2D_DESC texElementDesc;
		_terrainDesc.materials[0]->GetHeightMap()->GetTexture()->GetDesc(&texElementDesc);


		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = _terrainDesc.materials.size();
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> texArray;
		DEVICE->CreateTexture2D(&texArrayDesc, 0, texArray.GetAddressOf());

		for (uint32 texElement = 0; texElement < _terrainDesc.materials.size(); ++texElement)
		{
			// for each mipmap level...
			for (uint32 mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
			{
				if (_terrainDesc.materials[texElement] == nullptr) continue;
				if (_terrainDesc.materials[texElement]->GetHeightMap() == nullptr) continue;

				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				CHECK(DC->Map(_terrainDesc.materials[texElement]->GetHeightMap()->GetTexture().Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

				DC->UpdateSubresource(texArray.Get(),
					::D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
					0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

				DC->Unmap(_terrainDesc.materials[texElement]->GetHeightMap()->GetTexture().Get(), mipLevel);
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = _terrainDesc.materials.size();

		CHECK(DEVICE->CreateShaderResourceView(texArray.Get(), &viewDesc, _heightArraySrv.GetAddressOf()));
	}
}
ComPtr<ID3D11ShaderResourceView> Terrain::GetTextureSRV(int index)
{
	if (_terrainDesc.materials[index] == nullptr) return nullptr;
	return _terrainDesc.materials[index]->GetDiffuseMap()->GetResourceView();
}
void Terrain::SaveMapData(string fileName)
{
	string path = "../Resources/Map/" + fileName + ".raw";
	filesystem::path filePath(path);
	
	if (!filesystem::exists(filePath.parent_path()))
		filesystem::create_directories(filePath.parent_path());

	ofstream fout;
	fout.open(path,ios_base::binary);

	fout.write((char*)_rawVertices.data(), _rawVertices.size() * sizeof(Vector3));
	fout.write((char*)_heightMap.data(), _heightMap.size() * sizeof(float));
	fout.write((char*)_blendMap.data(), _blendMap.size() * sizeof(Vector4));
	fout.close();

	_heightChanged = true;
	_blendChanged = true;

}
void Terrain::LoadMapData(string fileName)
{
	string path = "../Resources/Map/" + fileName + ".raw";
	filesystem::path filePath(path);
	if (!filesystem::exists(path)) return;

	ifstream finput;
	finput.open(path, ios_base::binary);
	

	if (finput)
	{
		finput.read((char*)_rawVertices.data(), _rawVertices.size() * sizeof(Vector3));
		finput.read((char*)_heightMap.data(), _heightMap.size() * sizeof(float));
		finput.read((char*)_blendMap.data(), _blendMap.size() * sizeof(Vector4));

		finput.close();
	}
	_heightChanged = true;
	_blendChanged = true;

	auto terrainCollider = _gameObject.lock()->GetCompoent<TerrainCollider>();
	if (terrainCollider != nullptr)
		terrainCollider->CookingCollider();
}
void Terrain::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::C))
	{
		_lockFrustums = !_lockFrustums;
	}
}
void Terrain::Render()
{
	if (_material == nullptr) return;
	if (_material->GetShader() == nullptr) return;

	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	// TransformBuffer
	TransformDesc transform = {};
	{
		transform.W = _gameObject.lock()->GetTransform()->GetWorldMatrix();
		_material->GetShader()->PushTransformData(transform);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::Q))
	{
		_lockFrustums = !_lockFrustums;
	}

	// Frustum
	{
		if (!_lockFrustums) {
			MathUtils::ExtractFrustumPlanes(_frustums, Camera::S_MatView * Camera::S_MatProjection);
		}
		_material->GetShader()->GetVector("worldFrustumPlanes")->SetFloatVectorArray(
			reinterpret_cast<float*>(_frustums), 0,6);
	}

	// Brush
	_material->GetShader()->GetScalar("ShowBrush")->SetBool(_isShowBrush);
	if (_isShowBrush)
	{
		_material->GetShader()->GetVariable("MousePos")->SetRawValue(&_mousePos,0,sizeof(Vector3));
		_material->GetShader()->GetScalar("BrushSize")->SetFloat(_brushSize);
	}

	// HeightMap
	D3D11_MAPPED_SUBRESOURCE mappedData;
	{
		if (_heightChanged)
		{
			DC->Map(_heightMapTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			float* data = reinterpret_cast<float*>(mappedData.pData);

			for (int i = 0; i < _heightMap.size(); i++)
			{
				data[i] = _heightMap[i];
			}

			DC->Unmap(_heightMapTexture.Get(), 0);
			_heightChanged = false;
		}
	}

	// BlendMap
	if (_blendChanged) {
		DC->Map(_blendMapTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		Vector4* data = reinterpret_cast<Vector4*>(mappedData.pData);

		for (int i = 0; i < _blendMap.size(); i++)
		{
			data[i] = _blendMap[i];
		}

		DC->Unmap(_blendMapTexture.Get(), 0);
		_blendChanged = false;
	}


	_material->GetShader()->GetSRV("TerrainHeightMap")->SetResource(_heightMapSrv.Get());
	_material->GetShader()->GetSRV("TerrainBlendMap")->SetResource(_blendMapSrv.Get());
	_material->GetShader()->GetSRV("DiffuseArray")->SetResource(_diffuseArraySrv.Get());
	_material->GetShader()->GetSRV("NormalArray")->SetResource(_normalArraySrv.Get());
	_material->GetShader()->GetSRV("HeightArry")->SetResource(_heightArraySrv.Get());


	_material->Update();

	uint32 stride = sizeof(VertexTerrainData);
	uint32 offset = 0;
	DC->IASetVertexBuffers(0, 1, _quadPatchVB.GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_quadPatchIB.Get(), DXGI_FORMAT_R32_UINT, 0);


	if (!CUR_SCENE->GetIsDrawShadowMap())
		_material->GetShader()->DrawIndexed(L"TerrainTech", _pass, _numPatchQuadFaces * 4);
	else
		_material->GetShader()->DrawIndexed(L"TerrainTech", 2, _numPatchQuadFaces * 4);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


}

// 포인트 위치에 가까울 수록 높이 멀 수록 0에 가깝게 조정된다.
void Terrain::ModifyHeight(uint32 index,int8 height, Vector3 point,float radius)
{
	uint32 sizeX = _terrainDesc.size.x;
	uint32 sizeZ = _terrainDesc.size.y;
	float cellSpacing = _terrainDesc.cellSpacing;

	for (int32 z = -radius / cellSpacing; z < radius / cellSpacing; z++)
	{
		for (int32 x = -radius / cellSpacing; x < radius / cellSpacing; x++)
		{
			uint32 idx = index + x + z * (sizeX+1);
			if (idx < 0 || idx >= _rawVertices.size()) continue;
			Vector3 vPos = _rawVertices[idx];
			vPos.y = point.y;
			float distance = Vector3::Distance(vPos, point);
			if ((radius - distance) < 0) continue;
			float h = height * 0.05f * ((radius - distance) / radius);

			float u = ((sizeX - 1) * cellSpacing * 0.5f + vPos.x) / (sizeX * cellSpacing);
			float v = ((sizeZ - 1) * cellSpacing * 0.5f + vPos.z) / (sizeZ * cellSpacing);

			if (u == 1 || v == 1) continue;
			int heightIndex = u * (sizeX)+v * (sizeZ)*sizeX;
			_heightMap[heightIndex] += h;

			_rawVertices[idx].y = _heightMap[heightIndex];
			_heightChanged = true;
		}
	}
	auto terrainCollider = _gameObject.lock()->GetCompoent<TerrainCollider>();
	if (terrainCollider != nullptr)
		terrainCollider->CookingCollider();
}
void Terrain::SmoothHeight(uint32 index, Vector3 point, float radius)
{
	uint32 sizeX = _terrainDesc.size.x;
	uint32 sizeZ = _terrainDesc.size.y;
	float cellSpacing = _terrainDesc.cellSpacing;

	for (int32 z = -radius / cellSpacing; z < radius / cellSpacing; z++)
	{
		for (int32 x = -radius / cellSpacing; x < radius / cellSpacing; x++)
		{
			uint32 idx = index + x + z * (sizeX + 1);
			if (idx < 0 || idx >= _rawVertices.size()) continue;
			Vector3 vPos = _rawVertices[idx];
			vPos.y = point.y;
			float distance = Vector3::Distance(vPos, point);
			if ((radius - distance) < 0) continue;

			float u = ((sizeX - 1) * cellSpacing * 0.5f + vPos.x) / (sizeX * cellSpacing);
			float v = ((sizeZ - 1) * cellSpacing * 0.5f + vPos.z) / (sizeZ * cellSpacing);

			if (u == 1 || v == 1) continue;
			int heightIndex = u * (sizeX)+v * (sizeZ)*sizeX;
			int hx = heightIndex % sizeX;
			int hz = heightIndex / sizeX;
			
			_heightMap[heightIndex] = Average(hx, hz);
			_rawVertices[idx].y = _heightMap[heightIndex];
			_heightChanged = true;
		}
	}
	auto terrainCollider = _gameObject.lock()->GetCompoent<TerrainCollider>();
	if (terrainCollider != nullptr)
		terrainCollider->CookingCollider();
}
void Terrain::ModifyBlend(uint32 index, int8 blendIndex, Vector3 point, float radius)
{
	uint32 sizeX = _terrainDesc.size.x;
	uint32 sizeZ = _terrainDesc.size.y;
	float cellSpacing = _terrainDesc.cellSpacing;
	for (int32 z = -radius / cellSpacing; z < radius / cellSpacing; z++)
	{
		for (int32 x = -radius / cellSpacing; x < radius / cellSpacing; x++)
		{
			uint32 idx = index + x + z * (sizeX + 1);
			if (idx < 0 || idx >= _rawVertices.size()) continue;
			Vector3 vPos = _rawVertices[idx];
			vPos.y = point.y;
			float distance = Vector3::Distance(vPos, point);
			if ((radius - distance) < 0) continue;
			float power = 0.2f * ((radius - distance) / radius);

			float u = ((sizeX - 1) * cellSpacing * 0.5f + vPos.x) / (sizeX * cellSpacing);
			float v = ((sizeZ - 1) * cellSpacing * 0.5f + vPos.z) / (sizeZ * cellSpacing);

			if (u == 1 || v == 1) continue;
			int textureIndex = u * (sizeX)+v * (sizeZ)*sizeX;

			if (blendIndex == 0)
				_blendMap[textureIndex].x += power;
			if (blendIndex == 1)
				_blendMap[textureIndex].y += power;
			if (blendIndex == 2)
				_blendMap[textureIndex].z += power;
			if (blendIndex == 3)
				_blendMap[textureIndex].w += power;

			_blendMap[textureIndex].Normalize();
			_blendChanged = true;
		}
	}
	
}
void Terrain::CalcNormal()
{
	for (int i = 0; i < _indices.size()-2; i++) 
	{
		int i0 = _indices[i + 0];
		int i1 = _indices[i + 1];
		int i2 = _indices[i + 2];

		Vector3 pos0 = _vertices[i0].position;
		Vector3 pos1 = _vertices[i1].position;
		Vector3 pos2 = _vertices[i2].position;

		Vector3 e0 = pos1 - pos0;
		Vector3 e1 = pos2 - pos0;
		Vector3 faceNormal = e1.Cross(e0);

		/*_vertices[i0].normal = faceNormal;
		_vertices[i1].normal = faceNormal;
		_vertices[i2].normal = faceNormal;*/
	}

	for (int i = 0; i < _vertices.size(); i++) {

	}
		//_vertices[i].normal.Normalize();
}

void Terrain::RenrerTesselation()
{
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	uint32 stride = sizeof(VertexTerrainData);
	uint32 offset = 0;
	DC->IASetVertexBuffers(0, 1, _quadPatchVB.GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_quadPatchIB.Get(), DXGI_FORMAT_R16_UINT, 0);


	//GlobalBuffer
	GlobalDesc global = {};
	global.V = Camera::S_MatView;
	global.P = Camera::S_MatProjection;
	global.VP = global.V * global.P;
	global.VInv = global.V.Invert();
	_material->GetShader()->PushGlobalData(global);


	MaterialDesc desc = {};
	{
		desc.diffuse = { 1,1,1,1 };
		desc.ambient = { 1,1,1,1 };
		desc.emissive = { 0,0,0,0 };
		desc.specular = { 0.3f,0.3f,0.3f,1 };
	}
	_material->GetShader()->PushMaterialData(desc);


	_material->GetShader()->GetSRV("HeightMap")->SetResource(_heightMapSrv.Get());
	_material->GetShader()->GetSRV("DiffuseMap")->SetResource(_terrainDesc.materials[0]->GetDiffuseMap()->GetResourceView().Get());
	//_material->GetShader()->GetScalar("gTexelCellSpaceU")->SetFloat(1.0f / _sizeX);
	//_material->GetShader()->GetScalar("gTexelCellSpaceV")->SetFloat(1.0f / _sizeZ);
	//_material->GetShader()->GetScalar("gWorldCellSpace")->SetFloat(_cellSpacing);

	
	TransformDesc transformDesc;
	{
		transformDesc.W = _gameObject.lock()->GetTransform()->GetWorldMatrix();
	}
	_material->GetShader()->PushTransformData(transformDesc);
	_material->GetShader()->DrawIndexed(L"TerrainTech", 2, _numPatchQuadFaces * 4, 0, 0);

	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}
float Terrain::Average(int32 i, int32 j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for (int32 m = j - 1; m <= j + 1; ++m)
	{
		for (int32 n = i - 1; n <= i + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += _heightMap[m * _terrainDesc.size.x + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

bool Terrain::InBounds(int32 i, int32 j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int32)_terrainDesc.size.y&&
		j >= 0 && j < (int32)_terrainDesc.size.x;
}
void Terrain::Smooth()
{
	std::vector<float> dest(_heightMap.size());

	for (uint32 i = 0; i < _terrainDesc.size.y; ++i)
	{
		for (uint32 j = 0; j < _terrainDesc.size.x; ++j)
		{
			dest[i * _terrainDesc.size.y + j] = Average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	_heightMap = dest;
}
void Terrain::CalcPatchBoundsY(uint32 i, uint32 j)
{
	uint32 x0 = j * CellsPerPatch;
	uint32 x1 = (j + 1) * CellsPerPatch;

	uint32 y0 = i * CellsPerPatch;
	uint32 y1 = (i + 1) * CellsPerPatch;

	float minY = FLT_MAX;
	float maxY = -FLT_MAX;

	for (uint32 y = y0; y <= y1; ++y)
	{
		for (uint32 x = x0; x <= x1; ++x)
		{
			uint32 k = y * _terrainDesc.size.x + x;
			minY = min(minY, _heightMap[k]);
			maxY = max(maxY, _heightMap[k]);
		}
	}

	uint32 patchID = i * (_numPatchVertexCols - 1) + j;
	_patchBoundsY[patchID] = Vector2(minY, maxY);
}
void Terrain::BuildQuadPatchVB()
{
	_vertices.resize(_numPatchVertexCols * _numPatchVertexRows);

	float halfWidth = 0.5f * _terrainDesc.cellSpacing * (_terrainDesc.size.x);
	float halfDepth = 0.5f * _terrainDesc.cellSpacing * (_terrainDesc.size.y);

	float patchWidth = (_terrainDesc.cellSpacing * _terrainDesc.size.x )/ (_numPatchVertexCols - 1);
	float patchDepth = (_terrainDesc.cellSpacing * _terrainDesc.size.y  )/ (_numPatchVertexRows - 1);
	float du = 1.0f / (_numPatchVertexCols - 1);
	float dv = 1.0f / (_numPatchVertexRows - 1);

	for (uint32 i = 0; i < _numPatchVertexRows; i++)
	{
		float z = -halfDepth + i * patchDepth;
		for (uint32 j = 0; j < _numPatchVertexCols; j++)
		{
			float x = -halfWidth + j * patchWidth;

			_vertices[j * _numPatchVertexCols + i].position = Vector3(x, 0, z);

			_vertices[j * _numPatchVertexCols + i].uv.x = j * du;
			_vertices[j * _numPatchVertexCols + i].uv.y = i * dv;
		}
	}

	/*for (uint32 i = 0; i < _numPatchVertexRows - 1; ++i)
	{
		for (uint32 j = 0; j < _numPatchVertexCols - 1; ++j)
		{
			uint32 patchID = i * (_numPatchVertexCols - 1) + j;
			_vertices[i + _numPatchVertexCols + j].height = _patchBoundsY[patchID];
		}
	}*/


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VertexTerrainData) * _vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &_vertices[0];
	HRESULT hr = DEVICE->CreateBuffer(&vbd, &vinitData, _quadPatchVB.GetAddressOf());
	CHECK(hr);
}
void Terrain::BuildQuadPatchIB()
{
	_indices.resize(_numPatchQuadFaces * 4); // 4 indices per quad face

	// Iterate over each quad and compute indices.
	int32 k = 0;
	for (uint32 i = 0; i < _numPatchVertexRows - 1; ++i)
	{
		for (uint32 j = 0; j < _numPatchVertexCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			_indices[k] = i * _numPatchVertexCols + j;
			_indices[k + 1] = i * _numPatchVertexCols + j + 1;

			// Bottom row of 2x2 quad patch
			_indices[k + 2] = (i + 1) * _numPatchVertexCols + j;
			_indices[k + 3] = (i + 1) * _numPatchVertexCols + j + 1;

			k += 4; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32) * _indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; 
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &_indices[0];
	HRESULT hr =DEVICE->CreateBuffer(&ibd, &iinitData, _quadPatchIB.GetAddressOf());
	CHECK(hr);
}
void Terrain::BuildHeightSRV()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = _terrainDesc.size.x;
	texDesc.Height = _terrainDesc.size.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &_heightMap[0];
	data.SysMemPitch = (_terrainDesc.size.x) * sizeof(float);
	data.SysMemSlicePitch = 0;

	HRESULT hr = DEVICE->CreateTexture2D(&texDesc, &data, _heightMapTexture.GetAddressOf());
	CHECK(hr);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = DEVICE->CreateShaderResourceView(_heightMapTexture.Get(), &srvDesc, _heightMapSrv.GetAddressOf());
	CHECK(hr);

}
void Terrain::BuildBlendSRV()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = _terrainDesc.size.x;
	texDesc.Height = _terrainDesc.size.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &_blendMap[0];
	data.SysMemPitch = (_terrainDesc.size.x) * sizeof(Vector4);
	data.SysMemSlicePitch = 0;

	HRESULT hr = DEVICE->CreateTexture2D(&texDesc, &data, _blendMapTexture.GetAddressOf());
	CHECK(hr);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = DEVICE->CreateShaderResourceView(_blendMapTexture.Get(), &srvDesc, _blendMapSrv.GetAddressOf());
	CHECK(hr);

}
