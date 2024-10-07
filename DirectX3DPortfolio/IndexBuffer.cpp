#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::Create(const vector<uint32>& indices)
{
	_stride = sizeof(uint32);
	_offset = 0;
	_count = indices.size();

	D3D11_BUFFER_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	{
		desc.ByteWidth = _stride * _count;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}

	D3D11_SUBRESOURCE_DATA data;
	::ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices.data();

	HRESULT hr = DEVICE->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hr);
}

