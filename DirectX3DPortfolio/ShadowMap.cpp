#include "pch.h"
#include "ShadowMap.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "GameObject.h"
#include "Terrain.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Light.h"

ShadowMap::ShadowMap()
{
	_shader = RESOURCE->Get<Shader>(L"Default");

	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;
	_viewport.Width = 2048;
	_viewport.Height = 2048;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;


	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = static_cast<UINT>(_viewport.Width);
	texDesc.Height = static_cast<UINT>(_viewport.Height);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	CHECK(DEVICE->CreateTexture2D(&texDesc, nullptr, _depthMap.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	CHECK(DEVICE->CreateDepthStencilView(_depthMap.Get(), &dsvDesc, _depthMapDSV.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	
	CHECK(DEVICE->CreateShaderResourceView(_depthMap.Get(), &srvDesc, _depthMapSRV.GetAddressOf()));
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::BindDsvAndSetNullRenderTarget()
{
	DC->RSSetViewports(1, &_viewport);
	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	DC->OMSetRenderTargets(1, renderTargets, _depthMapDSV.Get());
	DC->ClearDepthStencilView(_depthMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::BakeShadowMap()
{
	if (CUR_SCENE->GetLight()== nullptr) return;

	
	float radius = 20;

	CUR_SCENE->SetDrawShadowMap(true);
	Vector3 lightDir = CUR_SCENE->GetLight()->GetLightDesc().direction;
	Vector3 lightPos = -2.0f * radius * lightDir;
	Vector3	lightTarget = Vector3::Zero;

	if (lightDir == Vector3::Zero) return;

	Vector3 right = (lightTarget - lightPos).Cross(-Vector3::Up);
	Vector3 up = (lightTarget - lightPos).Cross(right);

	Matrix V = XMMatrixLookAtLH(lightPos, lightTarget, Vector3::Up);


	Vector3 center = XMVector3TransformCoord(lightTarget, V);

	float l = center.x - radius;
	float b = center.y - radius;
	float n = center.z - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float f = center.z + radius;
	XMMATRIX P = ::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	
	Matrix T
	(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	Matrix S = V * P * T;

	_lightView = V;
	_lightProj = P;
	_shadowTrasnform = S;

	_shader->GetScalar("ShadowDraw")->SetBool(true);
	_shader->GetMatrix("ShadowView")->SetMatrix((float*)&_lightView);
	_shader->GetMatrix("ShadowProj")->SetMatrix((float*)&_lightProj);
	

	shared_ptr<Scene> curScene = CUR_SCENE;
	unordered_set<shared_ptr<GameObject>>& gameObjects = curScene->GetObjects();

	_vecForward.clear();

	for (auto gameObject : gameObjects)
	{
		gameObject->Render();


		if((gameObject->GetCompoent<MeshRenderer>() != nullptr) ||
			(gameObject->GetCompoent<SkinnedMeshRenderer>() != nullptr &&
				gameObject->GetCompoent<SkinnedMeshRenderer>()->GetRenderInstance()))
			_vecForward.push_back(gameObject);
	}

	INSTANCING->Render(_vecForward);


	_shader->GetMatrix("ShadowTransform")->SetMatrix((float*)&_shadowTrasnform);
	_shader->GetSRV("ShadowMap")->SetResource(DepthMapSRV().Get());
	CUR_SCENE->SetDrawShadowMap(false);
	_shader->GetScalar("ShadowDraw")->SetBool(false);
}