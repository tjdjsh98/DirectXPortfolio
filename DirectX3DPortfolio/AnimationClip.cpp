#include "pch.h"
#include "AnimationClip.h"
AnimationClip::AnimationClip() : ResourceBase(ResourceType::Animation)
{
}
AnimationClip::~AnimationClip()
{
}
std::shared_ptr<ModelKeyframe> AnimationClip::GetKeyframe(const wstring& name)
{
	auto findIt = _desc.keyframes.find(name);
	if (findIt == _desc.keyframes.end())
		return nullptr;

	return findIt->second;
}
