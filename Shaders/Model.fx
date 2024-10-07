#include "Global.fx"
#include "Light.fx"

#ifndef _MODEL_FX_
#define _MODEL_FX_

#define MAX_MODEL_TRANSFORM 250


////////////
// Struct //
////////////
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
	TweenDesc TweenFrames[MAX_INSTANCE_MESH];
};


Texture2DArray TransformMap;

// 모델이 애니메이션 실행때 사용
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
	nextFrame[0] = TweenFrames[input.instanceID].curr.nextFrame;
	ratio[0] = TweenFrames[input.instanceID].curr.ratio;

	animIndex[1] = TweenFrames[input.instanceID].next.animIndex;
	currFrame[1] = TweenFrames[input.instanceID].next.currFrame;
	nextFrame[1] = TweenFrames[input.instanceID].next.nextFrame;
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

		n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
		n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
		n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
		n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
		next = matrix(n0, n1, n2, n3);

		matrix result = lerp(curr, next, ratio[0]);

		// 다음 애니메이션
		if (animIndex[1] >= 0)
		{
			c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
			c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
			c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
			c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
			curr = matrix(c0, c1, c2, c3);

			n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
			n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
			n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
			n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
			next = matrix(n0, n1, n2, n3);

			matrix nextResult = lerp(curr, next, ratio[1]);
			result = lerp(result, nextResult, TweenFrames[input.instanceID].tweenRatio);
		}

		transform += mul(weights[i], result);
	}

	return transform;
}

// 모델이 애니메이션 실행 안 할 때 사용
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

MeshOutput VS(VertexTextureNormalTangent input)
{
	MeshOutput output;
	output.position = mul(input.position,W);
	output.worldPosition = output.position;
	if(!ShadowDraw)
		output.position = mul(output.position,VP);
	else
		output.position = mul(output.position, ShadowView * ShadowProj);

	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}

// 모델 인스턴스
MeshOutput Model_VS(VertexTextureNormalTangentBlend input)
{
	MeshOutput output;

	Matrix boneMat = GetBoneMatrix(input);
	output.position = mul(input.position, boneMat);
	output.worldPosition = output.position;
	if (!ShadowDraw)
		output.position = mul(output.position, VP);
	else
		output.position = mul(output.position, mul(ShadowView,ShadowProj));
	output.normal = mul(input.normal, boneMat);

	output.shadowPosH = mul(float4(output.worldPosition.xyz, 1), ShadowTransform);
	output.uv = input.uv;

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
	if (!ShadowDraw)
		output.position = mul(output.position, VP);
	else
		output.position = mul(output.position, mul(ShadowView, ShadowProj));


	output.shadowPosH = mul(float4(output.worldPosition.xyz, 1), ShadowTransform);

	output.uv = input.uv;
	output.normal = mul(input.normal, boneMat);

	return output;
}

float4 Model_PS(MeshOutput input) :SV_TARGET
{
	float shadow = ComputeShadowMap(input.shadowPosH);
	float4 color = ComputeLight(DiffuseMap.Sample(LinearSampler,input.uv),input.normal, input.uv, input.worldPosition);

	return color * (shadow + 0.2f);
}


#endif