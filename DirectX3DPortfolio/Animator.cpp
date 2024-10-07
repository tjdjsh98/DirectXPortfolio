#include "pch.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "Model.h"
#include "GameObject.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Shader.h"
#include "SkinnedMeshRenderer.h"
#include "ModelMesh.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_tweenDesc.curr.animIndex = rand() % 3;
}

Animator::~Animator()
{
}

void Animator::Update()
{
	if (_model == nullptr)return;

	UpdateTween();

	for (auto child : GetTransform()->GetChildren())
	{
		auto mesh = child->GetGameObject()->GetCompoent<SkinnedMeshRenderer>();
		if (mesh)
		{
			mesh->SetAnimator(shared_from_this());
		}
	}

	//clock_t start, finish;
	//start = clock();

	/*for (auto mesh : _model->GetInstancedMeshes()) 
	{
		mesh->GetCompoent<SkinnedMeshRenderer>()->SetAnimator(shared_from_this());
	}*/

	//if (_cacheBones.size() == 0)
	//{
	//	_cacheBones = _model->GetInstancedBones();
	//	return;
	//}

	//for (int b = 0; b < _cacheBones.size(); b++)
	//{
	//	if (_cacheBones[b] == nullptr) break;
	//	shared_ptr<ModelKeyframe> frame = _clip->GetKeyframe(_cacheBones[b]->GetName());
	//	ModelKeyframeData data = frame->transforms[_tweenDesc.curr.currFrame];

	//	// 0.001-2
	//	Matrix m = Matrix::CreateScale(data.scale);
	//	m *= Matrix::CreateFromQuaternion(data.rotation);
	//	m *= Matrix::CreateTranslation(data.translation);
	//	_cacheBones[b]->GetTransform()->SetLocalMatrix(m,true);

	//	// 0.002-3
	//	/*_cacheBones[b]->GetTransform()->SetLocalScale(data.scale);
	//	_cacheBones[b]->GetTransform()->SetLocalPosition(data.translation);
	//	_cacheBones[b]->GetTransform()->SetLocalRotation(Transform::ToEulerAngles(data.rotation));*/


	//}
	//finish = clock();
	//auto duration = (finish - start) / CLOCKS_PER_SEC;
	//int a = 0;
	
}

void Animator::UpdateTween() 
{
	TweenDesc& desc = GetTweenDesc();

	desc.curr.sumTime += DT;
	{
		shared_ptr<AnimationClip> currentAnimation = _model->GetAnimationByIndex(desc.curr.animIndex);
		shared_ptr<AnimationClip> nextAnimation = _model->GetAnimationByIndex(desc.next.animIndex);
		if (currentAnimation)
		{
			float timePerFrame = 1 / (currentAnimation->GetAnimationDesc().frameRate * desc.curr.speed);
			if (desc.curr.sumTime >= timePerFrame)
			{
				desc.curr.sumTime = 0;
				desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnimation->GetAnimationDesc().frameCount;
				desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnimation->GetAnimationDesc().frameCount;
			}
			desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
		}
		if (nextAnimation)
		{
			desc.next.sumTime += DT;
			desc.tweenSumTime += DT;
			float timePerFrame = 1 / (nextAnimation->GetAnimationDesc().frameRate * desc.curr.speed);
			if (desc.next.sumTime >= timePerFrame)
			{
				desc.next.sumTime = 0;
				desc.next.currFrame = (desc.next.currFrame + 1) % nextAnimation->GetAnimationDesc().frameCount;
				desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnimation->GetAnimationDesc().frameCount;
			}
			desc.next.ratio = (desc.next.sumTime / timePerFrame);
			desc.tweenRatio = (desc.tweenSumTime / desc.tweenDuration);

			if (desc.tweenRatio >= 1)
			{
				desc.curr.animIndex = desc.next.animIndex;
				desc.curr.currFrame = desc.next.currFrame;
				desc.curr.nextFrame = desc.next.nextFrame;
				desc.curr.sumTime = desc.next.sumTime;
				desc.curr.ratio = desc.next.ratio;

				desc.next.animIndex = -1;
				desc.next.currFrame = 0;
				desc.next.nextFrame = 0;
				desc.next.ratio = 0;
				desc.next.sumTime = 0;
				desc.tweenSumTime = 0;
			}
		}
	}
}

void Animator::SetAnimationClip(int index)
{
	if (_model == nullptr) return;
	if (index < 0 || index >= _model->GetAnimationCount()) return;

	_tweenDesc.curr.animIndex = index;
}

void Animator::SetNextAnimationClip(int index)
{
	if (_model == nullptr) return;
	if (index < 0 || index >= _model->GetAnimationCount()) return;

	_tweenDesc.next.animIndex = index;
}

void Animator::CreateTexture()
{
	if (_model->GetAnimationCount() == 0)
		return;

	_animTransforms.resize(_model->GetAnimationCount());
	for (uint32 i = 0; i < _model->GetAnimationCount(); i++)
		CreateAnimationTransform(i);

	// Creature Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = _model->GetAnimationCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const uint32 dataSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;
		void* mallocPtr = ::malloc(pageSize * _model->GetAnimationCount());

		// 파편화된 데이터를 조립한다.
		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			uint32 startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
			{
				void* ptr = pageStartPtr + dataSize * f;
				::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
			}
		}

		// 리소스 만들기
		vector<D3D11_SUBRESOURCE_DATA> subResources(_model->GetAnimationCount());

		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		HRESULT hr = DEVICE->CreateTexture2D(&desc, subResources.data(), _texture.GetAddressOf());
		CHECK(hr);

		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = _model->GetAnimationCount();

		HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _srv.GetAddressOf());
		CHECK(hr);
	}
}

void Animator::CreateAnimationTransform(uint32 index)
{
	vector<Matrix> tempAnimBoneTransforms(MAX_MODEL_TRANSFORMS, Matrix::Identity);

	shared_ptr<AnimationClip> animation = _model->GetAnimationByIndex(index);

	for (uint32 f = 0; f < animation->GetAnimationDesc().frameCount; f++)
	{
		for (uint32 b = 0; b < _model->GetBoneCount(); b++)
		{
			shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b);

			Matrix matAnimation;

			shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);
			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->transforms[f];

				Matrix S, R, T;
				S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
				R = Matrix::CreateFromQuaternion(data.rotation);
				T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

				matAnimation = S * R * T;
			}
			else
			{
				matAnimation = Matrix::Identity;
			}
			Matrix toRootMatrix = bone->transform;
			Matrix invGlobal = toRootMatrix.Invert();

			int32 parentIndex = bone->parentIndex;

			Matrix matParent = Matrix::Identity;
			if (parentIndex >= 0)
				matParent = tempAnimBoneTransforms[parentIndex];

			tempAnimBoneTransforms[b] = matAnimation * matParent;

			_animTransforms[index].transforms[f][b] = invGlobal * tempAnimBoneTransforms[b];
		}
	}
}

void Animator::PushAnimationData(shared_ptr<Shader>& shader)
{
	if (_texture == nullptr)
		CreateTexture();

	if (_srv)
		shader->GetSRV("TransformMap")->SetResource(_srv.Get());
}
