#include "pch.h"
#include "InstancingBuffer.h"

InstancingBuffer::InstancingBuffer()
{
	CreateBuffer(MAX_MESH_INSTANCE);
}

InstancingBuffer::~InstancingBuffer()
{
}

void InstancingBuffer::CreateBuffer(uint32 maxCount)
{
	_maxCount = maxCount;
	_instanceBuffer = make_shared<VertexBuffer>();

	vector<InstancingData> temp(maxCount);
	_instanceBuffer->Create(temp,1,true);
}

void InstancingBuffer::ClearData()
{
	_datas.clear();
}

void InstancingBuffer::AddData(InstancingData data)
{
	_datas.push_back(data);
}

void InstancingBuffer::PushData()
{
	const uint32 dataCount = GetCount();

	if (_maxCount < dataCount)
		CreateBuffer(dataCount);

	D3D11_MAPPED_SUBRESOURCE subResource;
	
	DC->Map(_instanceBuffer->GetBuffer().Get() ,0 ,D3D11_MAP_WRITE_DISCARD ,0, &subResource);
	{
		::memcpy(subResource.pData, _datas.data(), sizeof(InstancingData) * dataCount);
	}
	DC->Unmap(_instanceBuffer->GetBuffer().Get(), 0);

	_instanceBuffer->PushData();
}
