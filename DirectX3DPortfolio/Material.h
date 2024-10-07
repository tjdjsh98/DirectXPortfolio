#pragma once
#include "ResourceBase.h"
#include "BindShaderDesc.h"

class Shader;
class Texture;

class Material : public ResourceBase
{
public:
	Material();
	~Material();


 	shared_ptr<Shader>& GetShader() { return _shader; }

	MaterialDesc& GetMaterialDesc() { return _desc; }
	shared_ptr<Texture> GetDiffuseMap() { return _diffuseMap; }
	shared_ptr<Texture> GetNormalMap() { return _normalMap; }
	shared_ptr<Texture> GetSpecularMap() { return _specularMap; }
	shared_ptr<Texture> GetHeightMap() { return _heightMap; }

	void SetMaterialDesc(const MaterialDesc& desc) { _desc = desc; }
	void SetShader(shared_ptr<Shader> shader);
	void SetDiffuseMap(shared_ptr<Texture> diffuseMap) { _diffuseMap = diffuseMap; }
	void SetHeightMap(shared_ptr<Texture> heightMap) { _heightMap = heightMap; }
	void SetNormalMap(shared_ptr<Texture> normalMap) { _normalMap = normalMap; }
	void SetSpecularMap(shared_ptr<Texture> specularMap) { _specularMap = specularMap; }

	void Update();

public:
	virtual void Load(const wstring& path);

private:

	MaterialDesc _desc;

	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _diffuseMap;
	shared_ptr<Texture> _normalMap;
	shared_ptr<Texture> _specularMap;
	shared_ptr<Texture> _heightMap;

	ComPtr<ID3DX11EffectShaderResourceVariable> _diffuseEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _normalEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _specularEffectBuffer;
};

