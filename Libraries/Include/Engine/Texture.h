#pragma once
#include "ResourceBase.h"

class Texture : public ResourceBase
{
public:
	Texture();
	~Texture();

	ComPtr<ID3D11ShaderResourceView> GetResourceView() { return _shaderResourceView; }
	const Vector2& GetSize() { return _size; }

	virtual void Load(const wstring& path) override;
	void SetSRV(ComPtr<ID3D11ShaderResourceView> srv) { _shaderResourceView = srv; }
	const ComPtr<ID3D11Texture2D> GetTexture();

	const DirectX::ScratchImage& GetInfo() { return _img; }
private:
	ComPtr<ID3D11Texture2D> _texture = nullptr;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;
	Vector2 _size = { 0, 0 };
	DirectX::ScratchImage _img = {};
};

