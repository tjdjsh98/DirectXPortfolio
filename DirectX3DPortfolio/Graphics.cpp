#include "pch.h"
#include "Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

void Graphics::Init(GameDesc& desc)
{
	_gameDesc = desc;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();
	CreateViewPort();
}


void Graphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC _swapChainDesc;
	::ZeroMemory(&_swapChainDesc, sizeof(_swapChainDesc));
	{
		_swapChainDesc.BufferDesc.Width = _gameDesc.width;
		_swapChainDesc.BufferDesc.Height = _gameDesc.height;
		_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		_swapChainDesc.SampleDesc.Count = 1;
		_swapChainDesc.SampleDesc.Quality = 0;
		_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_swapChainDesc.BufferCount = 1;
		_swapChainDesc.OutputWindow = _gameDesc.hWnd;
		_swapChainDesc.Windowed = TRUE;
		_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		_swapChainDesc.Flags = 0;
	}


	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&_swapChainDesc,
		_swapchain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

void Graphics::CreateRenderTargetView()
{
	HRESULT hr;
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());

	CHECK(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateDepthStencilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = static_cast<uint32>(_gameDesc.width);
		desc.Height = static_cast<uint32>(_gameDesc.height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = _device->CreateTexture2D(&desc, nullptr, _depthStencilTexture.GetAddressOf());
		CHECK(hr);
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = _device->CreateDepthStencilView(_depthStencilTexture.Get(), &desc, _depthStencilView.GetAddressOf());
		CHECK(hr);
	}

}

void Graphics::CreateViewPort()
{
	_viewPort.TopLeftX = 0;
	_viewPort.TopLeftY = 0;
	_viewPort.Width = _gameDesc.width;
	_viewPort.Height = _gameDesc.height;
	_viewPort.MinDepth = 0;
	_viewPort.MaxDepth = 1;
}

void Graphics::BindDefaultDepthStencil()
{
}

void Graphics::BeginRender()
{
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), (float*)(&_gameDesc.clearColor));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_deviceContext->RSSetViewports(1,&_viewPort);
}

void Graphics::EndRender()
{
	HRESULT hr = _swapchain->Present(0, 0);
	CHECK(hr);

}
