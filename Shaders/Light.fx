#ifndef _LIGHT_FX_
#define _LIGHT_FX_

#include "Global.fx"

////////////
// Struct //
////////////

struct LightDesc
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 emissive;
	float3 direction;
	float padding;
};

struct MaterialDesc
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 emissive;
};

/////////////////
// ConstBuffer //
/////////////////

cbuffer LightBuffer
{
	LightDesc GlobalLight;
};

cbuffer MaterialBuffer
{
	MaterialDesc Material;
};
////////////
// Global //
////////////

bool ShadowDraw;
matrix ShadowView;
matrix ShadowProj;
matrix ShadowTransform;

////////////////
// Rasterizer //
////////////////

RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 100000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};


/////////
// SRV //
/////////

Texture2D ShadowMap;

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D HeightMap;

float4 ComputeLight(float4 color, float3 normal, float2 uv, float3 worldPosition)
{
	float4 ambientColor = 0;
	float4 diffuseColor = 0;
	float4 specularColor = 0;
	float4 emissiveColor = 0;


	// Ambient
	{
		float4 ambient = GlobalLight.ambient * Material.ambient;
		ambientColor = color * ambient;
	}

	// diffuseColor
	{
		float value = dot(-GlobalLight.direction, normalize(normal));
		diffuseColor = color * Material.diffuse * GlobalLight.diffuse * value;
	}

	// SpcularColor
	{
		float3 R = GlobalLight.direction - (2 * normal * dot(GlobalLight.direction, normal));
		R = normalize(R);

		float3 cameraPosition = CameraPosition();
		float3 E = normalize(cameraPosition - worldPosition);

		float value = saturate(dot(E, R));
		float specular = pow(value, 10);

		specularColor = GlobalLight.specular * Material.specular * specular;
	}

	// Emissive
	{
		float3 cameraPosition = CameraPosition();
		float3 E = normalize(cameraPosition - worldPosition);

		float value = saturate(dot(E, normal));
		float emissive = 1.0f - value;

		// min, max, x
		emissive = smoothstep(0.0f, 1.0f, emissive);
		emissive = pow(emissive, 2);

		emissiveColor = GlobalLight.emissive * Material.emissive * emissive;
	}
	return ambientColor + diffuseColor + specularColor + emissiveColor;
}

float4 ComputePointLight(float4 color, float3 normal, float2 uv, float3 worldPosition)
{

}

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS;
};

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float3 ComputeShadowMap(float4 shadowPosH)
{
	//shadowPosH.xyz /= shadowPosH.w;

	float depth = shadowPosH.z;

	const float dx = SMAP_DX;

	

	float percentLit = 0.0f;

	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	for (int i = 0; i < 9; ++i)
	{
		percentLit += ShadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}
	return percentLit /= 9.0f;
}

void ComputeNormalMapping(float4 normalMap, inout float3 normal, float3 tangent, float2 uv)
{
	if (any(normalMap.rgb) == false)
		return;

	float3 N = normalize(normal);		//z
	float3 T = normalize(tangent);		//x
	float3 B = normalize(cross(N, T));	//y
	float3x3 TBN = float3x3(T, B, N);

	// [0,1] 범위에서 [-1,1] 범위로 변환
	float3 tangentSpaceNormal = (normalMap.rgb * 2.0f - 1.0f);
	float3 worldNormal = mul(tangentSpaceNormal, TBN);

	normal = worldNormal;
}

#endif