#pragma once

class Shader;

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	ComPtr<ID3D11ShaderResourceView> DepthMapSRV() { return _depthMapSRV; }
	ComPtr<ID3D11Texture2D> GetDepthMap() { return _depthMap; }
	void BindDsvAndSetNullRenderTarget();

	void BakeShadowMap();

private:
	ComPtr<ID3D11Texture2D> _depthMap;
	ComPtr<ID3D11ShaderResourceView> _depthMapSRV;
	ComPtr<ID3D11DepthStencilView> _depthMapDSV;

	D3D11_VIEWPORT _viewport;

private:
	// 전역변수 입력용 쉐이더
	shared_ptr<Shader> _shader;
	Matrix _lightView;
	Matrix _lightProj;
	Matrix _shadowTrasnform;

	vector<shared_ptr<GameObject>> _vecForward;
};

