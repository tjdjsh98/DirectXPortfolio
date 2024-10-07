#pragma once
#include "Shader.h"

template<typename T>
class EffectBuffer
{
public:
	EffectBuffer(const string& bufferName, shared_ptr<Shader> shader) : _bufferName(bufferName), _shader(shader) {};
	~EffectBuffer() {};
	void PushData(const T& desc);

private:
	string _bufferName;
	T _desc;

private:
	weak_ptr<Shader> _shader;
	shared_ptr<ConstantBuffer<T>> _buffer;
	ComPtr<ID3DX11EffectConstantBuffer> _effectBuffer;
};

template<typename T>
void EffectBuffer<T>::PushData(const T& desc)
{
	if (_buffer == nullptr)
	{
		_buffer = make_shared<ConstantBuffer<T>>();
		_buffer->Create();
		_effectBuffer = _shader->GetConstantBuffer(_bufferName);
	}

	_desc = desc;
	_buffer->CopyData(_desc);
	_effectBuffer->SetConstantBuffer(_buffer->GetBuffer().Get());
}