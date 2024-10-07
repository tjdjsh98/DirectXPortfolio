#include "Global.fx"
#include "Light.fx"
#ifndef _TERRAIN_FX_
#define _TERRAIN_FX_
#define HEIGHT_COUNT 496

cbuffer PerFrame
{
	float3 MousePos;
	float BrushSize;
	bool ShowBrush;
	float4 worldFrustumPlanes[6];

};
Texture2D TerrainHeightMap;
Texture2D TerrainBlendMap;

Texture2DArray DiffuseArray;
Texture2DArray NormalArray;
Texture2DArray HeightArry;


struct VertexIn
{
	float4 PosL     : POSITION;
	float2 Tex      : TEXCOORD0;
	float2 BoundsY  : TEXCOORD1;
};

struct VertexOut
{
	float4 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
	float2 BoundsY  : TEXCOORD1;
};



VertexOut Terrain_VS(VertexIn vin)
{
	VertexOut vout;

	// Terrain specified directly in world space.
	vout.PosW = vin.PosL;

	// Displace the patch corners to world space.  This is to make 
	// the eye to patch distance calculation more accurate.
	vout.PosW.y = HeightMap.SampleLevel(LinearSampler, vin.Tex, 0).r;

	// Output vertex attributes to next stage.
	vout.Tex = vin.Tex;
	vout.BoundsY = vin.BoundsY;

	return vout;
}

float CalcTessFactor(float3 p)
{
	float d = distance(p, CameraPosition());

	// max norm in xz plane (useful to see detail levels from a bird's eye).
	//float d = max( abs(p.x-gEyePosW.x), abs(p.z-gEyePosW.z) );

	float s = saturate((d - 10) / (150 - 10));

	return pow(2,(lerp(64, 0, s)));
}

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
	float3 n = abs(plane.xyz);

	// This is always positive.
	float r = dot(extents, n);

	// signed distance from center point to plane.
	float s = dot(float4(center, 1.0f), plane);

	// If the center point of the box is a distance of e or more behind the
	// plane (in which case s is negative since it is behind the plane),
	// then the box is completely in the negative half space of the plane.
	return (s + r) < 0.0f;
}

// Returns true if the box is completely outside the frustum.
bool AabbOutsideFrustumTest(float3 center, float3 extents, float4 frustumPlanes[6])
{
	for (int i = 0; i <6; ++i)
	{
		// If the box is completely behind any of the frustum planes
		// then it is outside the frustum.
		if (AabbBehindPlaneTest(center, extents, frustumPlanes[i]))
		{
			return true;
		}
	}

	return false;
}
struct PatchTess
{
	float EdgeTess[4]   : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};


PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	float minY = -100.0f;
	float maxY = 100.0f;

	float3 vMin = float3(patch[0].PosW.x, minY, patch[0].PosW.z);
	float3 vMax = float3(patch[3].PosW.x, maxY, patch[3].PosW.z);

	float3 boxCenter = 0.5f * (vMin + vMax);
	float3 boxExtents = 0.5f * (vMax - vMin);

	if (AabbOutsideFrustumTest(boxCenter, boxExtents, worldFrustumPlanes))
	{
		pt.EdgeTess[0] = 0;
		pt.EdgeTess[1] = 0;
		pt.EdgeTess[2] = 0;
		pt.EdgeTess[3] = 0;

		pt.InsideTess[0] = 0;
		pt.InsideTess[1] = 0;

		return pt;
	}

	float3 e0 = 0.5f * (patch[0].PosW + patch[2].PosW);
	float3 e1 = 0.5f * (patch[0].PosW + patch[1].PosW);
	float3 e2 = 0.5f * (patch[1].PosW + patch[3].PosW);
	float3 e3 = 0.5f * (patch[2].PosW + patch[3].PosW);
	float3  c = 0.25f * (patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);

	pt.EdgeTess[0] = CalcTessFactor(e0);
	pt.EdgeTess[1] = CalcTessFactor(e1);
	pt.EdgeTess[2] = CalcTessFactor(e2);
	pt.EdgeTess[3] = CalcTessFactor(e3);

	pt.InsideTess[0] = CalcTessFactor(c);
	pt.InsideTess[1] = pt.InsideTess[0];

	return pt;
}
struct HullOut
{
	float4 PosW     : POSITION;
	float2 Tex      : TEXCOORD0;
};

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	// Pass through shader.
	hout.PosW = p[i].PosW;
	hout.Tex = p[i].Tex;

	return hout;
}

struct DomainOut
{
	float4 PosH     : SV_POSITION;
	float4 PosW     : POSITION1;
	float2 Tex      : TEXCOORD0;
	float2 TiledTex : TEXCOORD1;
	float4 shadowPosH : SHADOWPOSITION;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("quad")]
DomainOut DS(PatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<HullOut, 4> quad)
{
	DomainOut dout;

	// Bilinear interpolation.
	float3 v1 = lerp(quad[0].PosW.xyz, quad[1].PosW.xyz, uv.x);
	float3 v2 = lerp(quad[2].PosW.xyz, quad[3].PosW.xyz, uv.x);
	float3 p = lerp(v1, v2, uv.y);

	float2 u1 = lerp(quad[0].Tex, quad[1].Tex, uv.x);
	float2 u2 = lerp(quad[2].Tex, quad[3].Tex, uv.x);
	dout.Tex = lerp(u1, u2, uv.y);
	dout.TiledTex = dout.Tex * 5;

	p.y += TerrainHeightMap.SampleLevel(LinearSampler, dout.Tex, 0).r;

	dout.PosW = mul(float4(p.xyz, 1), W);
	if(ShadowDraw)
		dout.PosH = mul(dout.PosW, mul(ShadowView, ShadowProj));
	else
		dout.PosH = mul(dout.PosW, VP);

	dout.shadowPosH = mul(float4(dout.PosW.xyz, 1), ShadowTransform);

	return dout;
}


float4 Terrain_PS(DomainOut pin) : SV_Target
{

	float4 t = TerrainBlendMap.Sample(LinearSampler, pin.Tex);

	float2 leftTex = pin.Tex + float2(-1 / 128.0f, 0.0f);
	float2 rightTex = pin.Tex + float2(1 / 128.0f, 0.0f);
	float2 bottomTex = pin.Tex + float2(0.0f, 1 / 128.0f);
	float2 topTex = pin.Tex + float2(0.0f, -1 / 128.0f);

	float leftY = TerrainHeightMap.SampleLevel(LinearSampler, leftTex, 0).r;
	float rightY = TerrainHeightMap.SampleLevel(LinearSampler, rightTex, 0).r;
	float bottomY = TerrainHeightMap.SampleLevel(LinearSampler, bottomTex, 0).r;
	float topY = TerrainHeightMap.SampleLevel(LinearSampler, topTex, 0).r;

	float3 tangent = normalize(float3(2.0f * 0.5f, rightY - leftY, 0.0f));
	float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * 0.5f));
	float3 normalW = cross(tangent, bitan);

	//// 노말 계산
	//float4 n0 = NormalArray.Sample(LinearSampler, float3(pin.Tex, 0.0f));
	//float4 n1 = NormalArray.Sample(LinearSampler, float3(pin.Tex, 1.0f));
	//float4 n2 = NormalArray.Sample(LinearSampler, float3(pin.Tex, 2.0f));
	//float4 n3 = NormalArray.Sample(LinearSampler, float3(pin.Tex, 3.0f));

	//float4 normal = n0;
	//normal = lerp(normal, n1, t.g);
	//normal = lerp(normal, n2, t.b);
	//normal = lerp(normal, n3, t.w);

	//ComputeNormalMapping(float4(0,1,0,0), normalW, tangent, pin.Tex);

	float4 c0 = DiffuseArray.SampleLevel(LinearSampler, float3(pin.TiledTex, 0.0f),0);
	float4 c1 = DiffuseArray.SampleLevel(LinearSampler, float3(pin.TiledTex, 1.0f),0);
	float4 c2 = DiffuseArray.SampleLevel(LinearSampler, float3(pin.TiledTex, 2.0f),0);
	float4 c3 = DiffuseArray.SampleLevel(LinearSampler, float3(pin.TiledTex, 3.0f),0);


	float4 color = c0;
	color = lerp(color, c1, t.g);
	color = lerp(color, c2, t.b);
	color = lerp(color, c3, t.a);

	color = ComputeLight(color, normalW, pin.Tex, pin.PosW);


	if (ShowBrush)
	{
		if (distance(pin.PosW.xyz , MousePos) < BrushSize)
		{
			color += float4(0, 0.5f, 0, 0);
		}
	}
	float shadow = ComputeShadowMap(pin.shadowPosH);

	return shadow*color;
}

// Terrain Technique
technique11 TerrainTech
{
	// 일반 렌더링
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, Terrain_VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(CompileShader(ps_5_0, Terrain_PS()));
	}
	pass P1
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Terrain_VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(CompileShader(ps_5_0, Terrain_PS()));
	}
	pass P2
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Terrain_VS()));
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(NULL);
		SetRasterizerState(Depth);
	}
};

#endif