#pragma once

class Graphics
{
public:
	Graphics();
	~Graphics();

	void Init(GameDesc& desc);

public:
	ComPtr<ID3D11Device> GetDevice() { return _device;}
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext;}

public:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateViewPort();

	void BindDefaultDepthStencil();

public:
	void BeginRender();
	void EndRender();

public:
	float GetWidth() { return _gameDesc.width; }
	float GetHeight() { return _gameDesc.height; }

private:
	GameDesc _gameDesc;

private:
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapchain = nullptr;

	ComPtr<ID3D11RenderTargetView> _renderTargetView = nullptr;
	

	ComPtr<ID3D11Texture2D> _depthStencilTexture = nullptr;
	ComPtr<ID3D11DepthStencilView> _depthStencilView = nullptr;

	//Misc
	D3D11_VIEWPORT _viewPort;
};

