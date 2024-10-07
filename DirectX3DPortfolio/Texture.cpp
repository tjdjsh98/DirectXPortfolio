#include "pch.h"
#include "Texture.h"

Texture::Texture() : ResourceBase(ResourceType::Texture)
{
}

Texture::~Texture()
{
}

void Texture::Load(const wstring& path)
{
	DirectX::TexMetadata md;
	HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
	CHECK(hr);

	hr = ::CreateTextureEx(DEVICE.Get(),_img.GetImages(), _img.GetImageCount(), md,
		D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0, CREATETEX_DEFAULT,(ID3D11Resource**)_texture.GetAddressOf());
	
	CHECK(hr);

	hr = ::CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md,_shaderResourceView.GetAddressOf());

	CHECK(hr);

	_size.x = md.width;
	_size.y = md.height;
}

const ComPtr<ID3D11Texture2D> Texture::GetTexture()
{
	return _texture;
}