#pragma once

struct ModelKeyframeData
{
	float time;
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;
};

struct ModelKeyframe
{
	wstring boneName;
	vector<ModelKeyframeData> transforms;
};

struct AnimationDesc
{
	wstring name;
	float duration = 0.f;
	float frameRate = 0.f;
	uint32 frameCount = 0;
	unordered_map<wstring, shared_ptr< ModelKeyframe>> keyframes;
};


class AnimationClip : public ResourceBase
{
public:
	AnimationClip();
	~AnimationClip();
	shared_ptr<ModelKeyframe> GetKeyframe(const wstring& name);
	const AnimationDesc& GetAnimationDesc() { return _desc; }

	void SetAnimationDesc(AnimationDesc& desc) { _desc = desc; }
public:
	AnimationDesc _desc;

};


