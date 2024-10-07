#pragma once

class VertexBuffer
{
public:
	VertexBuffer(){};
	~VertexBuffer() { };


public:
	uint32 GetStride() { return _stride; }
	uint32 GetOffset() { return _offset; }
	uint32 GetCount() { return _count; }
	ComPtr<ID3D11Buffer> GetBuffer() { return _vertexBuffer; }

public:
	template<typename T>
	void Create(const vector<T>& vertices, uint32 slot = 0, bool cpuWrite = false, bool gpuWrite = false)
	{
		_stride = sizeof(T);
		_count = vertices.size();

		_slot = slot;
		_offset = 0;
		_cpuWrite = cpuWrite;
		_gpuWrite = gpuWrite;

		D3D11_BUFFER_DESC desc;
		::ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = sizeof(T) * vertices.size();
		if (cpuWrite == false && gpuWrite == false)
		{
			desc.Usage = D3D11_USAGE_IMMUTABLE; // GPU만 읽기 허용
		}
		else if (cpuWrite == true && gpuWrite == false)
		{
			desc.Usage = D3D11_USAGE_DYNAMIC; // CPU 쓰기 가능, GPU 읽기만 가능
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else if (cpuWrite == false && gpuWrite == true) // CPU Read, GPU Write
		{
			desc.Usage = D3D11_USAGE_DEFAULT; // GPU만 읽고 쓰기 허용
		}
		else
		{
			desc.Usage = D3D11_USAGE_STAGING; // GPU 읽고 쓰고 CPU로 일기
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		}

		D3D11_SUBRESOURCE_DATA data;
		::ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data();

		HRESULT hr = DEVICE->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());

		SUCCEEDED(hr);
	}

	void PushData()
	{
		DC->IASetVertexBuffers(_slot, 1, _vertexBuffer.GetAddressOf(), &_stride, &_offset);
	}

private:
	ComPtr<ID3D11Buffer> _vertexBuffer;

	uint32 _stride;
	uint32 _offset;
	uint32 _count;

	uint32 _slot;
	bool _cpuWrite;
	bool _gpuWrite;
};

