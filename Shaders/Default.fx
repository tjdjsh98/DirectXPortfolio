#include "Global.fx"
#include "Light.fx"
#include "Model.fx"
#include "Terrain.fx"

#define MAX_MODEL_TRANSFORM 250
#define MAX_INSTANCE_MESH 500
#define MAX_MODEL_INSTANCE 500


// 매쉬 인스턴스X
MeshOutput Mesh_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	output.position = mul(input.position, W);
	output.worldPosition = output.position;
	if (!ShadowDraw)
		output.position = mul(output.position, VP);
	else
		output.position = mul(output.position, ShadowView * ShadowProj);
	output.uv = input.uv;

	output.normal = mul(input.normal, W);
	output.tangent = mul(input.tangent, W);

	return output;
}

// 매쉬 인스턴스O
MeshOutput Mesh_Instancing_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	output.position = mul(input.position, input.world);
	output.worldPosition = output.position;
	if(ShadowDraw)
		output.position = mul(output.position, mul(ShadowView, ShadowProj));
	else
		output.position = mul(output.position, VP);
	output.uv = input.uv;
	output.normal = mul(input.normal, input.world);
	output.tangent = mul(input.tangent, input.world);
	output.shadowPosH = mul(float4(output.worldPosition.xyz,1), ShadowTransform);

	return output;
}

// 기즈모 인스턴스
MeshOutput Gizmo_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	output.position = mul(input.position, input.world);
	output.worldPosition = output.position;
	output.position = mul(output.position, VP);
	output.uv = input.uv;

	output.normal = mul(input.normal, W);
	output.tangent = mul(input.tangent, W);
	output.color = input.color;
	return output;
}

MeshOutput SkyBox_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	float4 viewPos = mul(float4(input.position.xyz, 0), V);
	float4 clipSpacePos = mul(viewPos, P);
	output.position = clipSpacePos;
	output.position.z = output.position.w * 0.9999f;
	output.worldPosition = output.position;
	output.uv = input.uv;
	output.normal = input.normal;
	output.tangent = input.tangent;

	return output;
}



float4 PS(MeshOutput input) :SV_TARGET
{
	float shadow = ComputeShadowMap(input.shadowPosH);
	float4 color = ComputeLight(DiffuseMap.Sample(LinearSampler,input.uv), input.normal, input.uv, input.worldPosition);
	return color* (shadow+ 0.2f );
}

float4 OnlyColor_PS(MeshOutput input) :SV_TARGET
{
	return float4(1,1,1,1);
}

float4 Gizmo_PS(MeshOutput input) :SV_TARGET
{
	return input.color;
}

float4 SkyBox_PS(MeshOutput input) : SV_TARGET
{
	return DiffuseMap.Sample(LinearSampler, input.uv) ;
}


// MeshRenderer
technique11 MeshTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, Mesh_Instancing_VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
	pass P1
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Mesh_Instancing_VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
	// Shadow
	pass P2
	{
		SetVertexShader(CompileShader(vs_5_0, Mesh_Instancing_VS()));
		SetPixelShader(NULL);
		SetRasterizerState(Depth);
	}
};

// Gizmo Technique
technique11 GizmoTech
{
	pass P0
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Gizmo_VS()));
		SetPixelShader(CompileShader(ps_5_0, Gizmo_PS()));
	}
};

// SkinnedMeshRenderer
technique11 SkinnedMeshTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, Model_VS()));
		SetPixelShader(CompileShader(ps_5_0, Model_PS()));
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, Animation_VS()));
		SetPixelShader(CompileShader(ps_5_0, Model_PS()));
	}
	// Shadow
	pass P2
	{
		SetVertexShader(CompileShader(vs_5_0, Model_VS()));
		SetPixelShader(NULL);
		SetRasterizerState(Depth);
	}
	pass P3
	{
		SetVertexShader(CompileShader(vs_5_0, Animation_VS()));
		SetPixelShader(NULL);
		SetRasterizerState(Depth);
	}
};
// SkyBox
technique11 SkyBoxTech
{
	pass P0
	{
		SetRasterizerState(NoCull);
		SetVertexShader(CompileShader(vs_5_0, SkyBox_VS()));
		SetPixelShader(CompileShader(ps_5_0, SkyBox_PS()));
	}
};

// Terrain T4
//
//// ShadowMap
//technique11 T5
//{
//	// MeshRender
//	pass P0
//	{
//		SetVertexShader(CompileShader(vs_5_0, Mesh_Instancing_VS()));
//		SetPixelShader(NULL);
//	}
//
//	// SkinnedMeshRender
//	pass P1
//	{
//		SetVertexShader(CompileShader(vs_5_0, Animation_VS()));
//		SetPixelShader(NULL);
//	}
//
//	// Terrain
//	pass P2
//	{
//		SetVertexShader(CompileShader(vs_5_0, Terrain_VS()));
//		SetHullShader(CompileShader(hs_5_0, HS()));
//		SetDomainShader(CompileShader(ds_5_0, DS()));
//		SetPixelShader(NULL);
//	}
//};