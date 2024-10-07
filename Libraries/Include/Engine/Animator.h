#pragma once
#include "Component.h"

struct AnimationClip;
struct TweenDesc;

struct AnimTransform
{
	using TransformArrayType = array<Matrix, MAX_MODEL_TRANSFORMS>;
	array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};
class Animator : public Component, public enable_shared_from_this<Animator>
{
public:
	Animator();
	virtual ~Animator();

	static ComponentType GetComponentType() { return ComponentType::Animator; }
	
	virtual void Update();

private:
	void UpdateTween();

public:
	void SetModel(shared_ptr<Model> model) { _model = model; }
	void SetClip(shared_ptr<AnimationClip> clip) { _clip = clip; }
	void Play() { _isPlay = true; }
	void Stop() { _isPlay = false; }
	void SetAnimationClip(int index);
	void SetNextAnimationClip(int index);

	bool GetIsPlay() { return _isPlay; }
	TweenDesc& GetTweenDesc() { return _tweenDesc; }
	shared_ptr<AnimationClip> GetClip() { return _clip; }
	shared_ptr<Model> GetModel() { return _model; }

private:
	void CreateTexture();
	void CreateAnimationTransform(uint32 index);

public:
	void PushAnimationData(shared_ptr<Shader>& shader);
	ComPtr<ID3D11Texture2D> GetAnimationTexture() { return _texture; }
	ComPtr<ID3D11ShaderResourceView> GetAnimtionSRV() {return _srv;}

private:
	shared_ptr<Model> _model = nullptr;
	shared_ptr<AnimationClip> _clip = nullptr;

	bool _isPlay = true;
	float _currentTime = 0;
	uint32 _frame = 0;

	TweenDesc _tweenDesc;

	// Cache
	vector<shared_ptr<GameObject>> _cacheBones;

private:
	vector<AnimTransform> _animTransforms;
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11ShaderResourceView> _srv;

};

