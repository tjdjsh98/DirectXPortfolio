#include "Global.fx"
#include "Light.fx"

#define MAX_MODEL_TRANSFORM 250
#define MAX_INSTANCE_MESH 500
#define MAX_MODEL_INSTANCE 500

cbuffer BoneBuffer
{
	matrix boneTransform[MAX_MODEL_TRANSFORM];
};
struct FrameDesc
{
	int animIndex;
	uint currFrame;
	uint nextFrame;
	float ratio;
	float sumTime;
	float speed;
	float2 padding;
};
struct TweenDesc
{
	float tweenDuration;
	float tweenRatio;
	float tweenSumTime;
	float padding;
	FrameDesc curr;
	FrameDesc next;
};
cbuffer InstancedTweenBuffer
{
	TweenDesc TweenFrames[MAX_MODEL_INSTANCE];
};
struct GizmoDesc
{
	float4 color;
};
cbuffer InstancedBuffer
{
	GizmoDesc Gizmos[MAX_INSTANCE_MESH];
};


Texture2DArray TransformMap;

// 매쉬 인스턴스X
MeshOutput Mesh_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	output.position = mul(input.position, W);
	output.worldPosition = output.position;
	output.position = mul(output.position, VP);
	output.uv = input.uv;

	output.normal = mul(input.normal, W);
	output.tangent = mul(input.tangent, W);

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


// 매쉬 인스턴스O
MeshOutput Mesh_Instancing_VS(VertexTextureNormalTangent input)
{
	MeshOutput output;

	output.position = mul(input.position, input.world);
	output.worldPosition = output.position;
	output.position = mul(output.position, VP);
	output.uv = input.uv;
	output.normal = mul(input.normal, input.world);
	output.tangent = mul(input.tangent, input.world);

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

matrix GetAnimationMatrix(VertexTextureNormalTangentBlend input)
{
	float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
	float weights[4] = { input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w };
	
	int animIndex[2];
	int currFrame[2];
	int nextFrame[2];
	float ratio[2];

	animIndex[0] = TweenFrames[input.instanceID].curr.animIndex;
	currFrame[0] = TweenFrames[input.instanceID].curr.currFrame;
	nextFrame[0] = TweenFrames[input.instanceID].next.currFrame;
	ratio[0] = TweenFrames[input.instanceID].curr.ratio;

	animIndex[1] = TweenFrames[input.instanceID].curr.animIndex;
	currFrame[1] = TweenFrames[input.instanceID].curr.currFrame;
	nextFrame[1] = TweenFrames[input.instanceID].next.currFrame;
	ratio[1] = TweenFrames[input.instanceID].curr.ratio;

	float4 c0, c1, c2, c3;
	float4 n0, n1, n2, n3;

	matrix curr = 0;
	matrix next = 0;
	matrix transform = 0;

	for (int i = 0; i < 4; i++)
	{
		c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
		c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
		c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
		c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
		curr = matrix(c0, c1, c2, c3);

		//n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
		//n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
		//n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
		//n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
		//next = matrix(n0, n1, n2, n3);

		//matrix result = lerp(curr, next, ratio[0]);

		//// 다음 애니메이션
		//if (animIndex[1] >= 0)
		//{
		//	c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
		//	c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
		//	c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
		//	c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
		//	curr = matrix(c0, c1, c2, c3);

		//	n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
		//	n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
		//	n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
		//	n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
		//	next = matrix(n0, n1, n2, n3);

		//	matrix nextResult = lerp(curr, next, ratio[1]);
		//	result = lerp(result, nextResult, TweenFrames[input.instanceID].tweenRatio);
		//}

		transform += mul(weights[i], curr);
	}

	return transform;
}
matrix GetBoneMatrix(VertexTextureNormalTangentBlend input)
{
	float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
	float weights[4] = { input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w };
	float4 c0, c1, c2, c3;

	matrix curr = 0;
	matrix transform = 0;

	for (int i = 0; i < 4; i++)
	{
		transform += mul(weights[i], boneTransform[indices[i]]);
	}

	return transform;
}

// 모델 인스턴스
MeshOutput Model_VS(VertexTextureNormalTangentBlend input)
{
	MeshOutput output;

	Matrix boneMat = GetBoneMatrix(input);
	output.position = mul(input.position, boneMat);
	output.worldPosition = output.position;
	output.position = mul(output.position,VP);

	/*output.normal = mul(localNormal, W);
	output.normal = mul(output.normal, VP);*/

	output.uv = input.uv;
	output.normal = input.normal;
	
	return output;
}
// 애니메이션 모델 인스턴스
MeshOutput Animation_VS(VertexTextureNormalTangentBlend input)
{
	MeshOutput output;

	Matrix boneMat = GetAnimationMatrix(input);
	output.position = mul(input.position, boneMat);
	output.position = mul(output.position, input.world);
	output.worldPosition = output.position;
	output.position = mul(output.position, VP);

	/*output.normal = mul(localNormal, W);
	output.normal = mul(output.normal, VP);*/

	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}

float4 PS(MeshOutput input) :SV_TARGET
{
	//ComputeNormalMapping(NormalMap.Sample(LinearSampler,input.uv),input.normal, input.tangent, input.uv);
	float4 color = ComputeLight(DiffuseMap.Sample(LinearSampler,input.uv), input.normal, input.uv, input.worldPosition);

	return color;
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
technique11 T0
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
	pass P2
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Animation_VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
	pass P3
	{
		SetVertexShader(CompileShader(vs_5_0, Mesh_VS()));
		SetPixelShader(CompileShader(ps_5_0, OnlyColor_PS()));
	}
};

// Gizmo Technique
technique11 T1
{
	pass P0
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Gizmo_VS()));
		SetPixelShader(CompileShader(ps_5_0, Gizmo_PS()));
	}
};

// SkinnedMeshRenderer
technique11 T2
{
	pass P0
	{
		SetRasterizerState(FillModeWireFrame);
		SetVertexShader(CompileShader(vs_5_0, Animation_VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};

// SkyBox
technique11 T3
{
	pass P0
	{
		SetRasterizerState(NoCull);
		SetVertexShader(CompileShader(vs_5_0, SkyBox_VS()));
		SetPixelShader(CompileShader(ps_5_0, SkyBox_PS()));
	}
};