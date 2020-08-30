#include "stdafx.h"
#include "Resource.h"

Resource::Resource(
	ID3D12Device* device,
	unsigned long long entrySize,
	RESOURCE_TYPE type,
	std::wstring name)
{
	this->entrySize = entrySize;
	this->type = type;
	this->name = name;

	D3D12_HEAP_TYPE d3d12HeapType;
	D3D12_RESOURCE_STATES startState;

	switch (type)
	{
	case RESOURCE_TYPE::UPLOAD:
		d3d12HeapType = D3D12_HEAP_TYPE_UPLOAD;
		startState = D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	case RESOURCE_TYPE::DEFAULT:
		d3d12HeapType = D3D12_HEAP_TYPE_DEFAULT;
		startState = D3D12_RESOURCE_STATE_COMMON;
		break;
	}

	this->SetupHeapProperties(d3d12HeapType);

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = this->entrySize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	this->CreateResource(device, &resourceDesc, nullptr, startState);
}

Resource::Resource(
	ID3D12Device* device,
	D3D12_RESOURCE_DESC* resourceDesc,
	D3D12_CLEAR_VALUE* clearValue,
	std::wstring name,
	D3D12_RESOURCE_STATES startState)
{
	this->type = RESOURCE_TYPE::DEFAULT;
	this->entrySize = resourceDesc->Width * resourceDesc->Height;
	this->name = name;

	D3D12_HEAP_TYPE d3d12HeapType = D3D12_HEAP_TYPE_DEFAULT;

	this->SetupHeapProperties(d3d12HeapType);

	this->CreateResource(device, resourceDesc, clearValue, startState);
}

Resource::~Resource()
{
	SAFE_RELEASE(&this->resource);
}

unsigned int Resource::GetSize() const
{
	return this->entrySize;
}

ID3D12Resource1* Resource::GetID3D12Resource1() const
{
	return this->resource;
}

ID3D12Resource1** Resource::GetID3D12Resource1PP()
{
	return &this->resource;
}

D3D12_GPU_VIRTUAL_ADDRESS Resource::GetGPUVirtualAdress() const
{
	return this->resource->GetGPUVirtualAddress();
}

void Resource::SetData(const void* data, unsigned int subResourceIndex)
{
	if (type == RESOURCE_TYPE::DEFAULT)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Trying to SetData into default heap\n");
		return;
	}

	void* dataBegin = nullptr;

	// Set up the heap data
	D3D12_RANGE range = { 0, 0 }; // We do not intend to read this resource on the CPU.

	this->resource->Map(subResourceIndex, &range, &dataBegin); // Get a dataBegin pointer where we can copy data to
	memcpy(dataBegin, data, this->entrySize);
	this->resource->Unmap(subResourceIndex, nullptr);
}

void Resource::SetData(const void* data, unsigned int subResourceIndex) const
{
	if (type == RESOURCE_TYPE::DEFAULT)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Trying to Map into default heap\n");
		return;
	}

	void* dataBegin = nullptr;

	// Set up the heap data
	D3D12_RANGE range = { 0, 0 }; // We do not intend to read this resource on the CPU.

	this->resource->Map(subResourceIndex, &range, &dataBegin); // Get a dataBegin pointer where we can copy data to
	memcpy(dataBegin, data, this->entrySize);
	this->resource->Unmap(subResourceIndex, nullptr);
}

void Resource::SetupHeapProperties(D3D12_HEAP_TYPE heapType)
{
	this->heapProperties.Type = heapType;
	this->heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	this->heapProperties.CreationNodeMask = 1; //used when multi-gpu
	this->heapProperties.VisibleNodeMask = 1; //used when multi-gpu
	this->heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
}

void Resource::CreateResource(
	ID3D12Device* device,
	D3D12_RESOURCE_DESC* resourceDesc,
	D3D12_CLEAR_VALUE* clearValue,
	D3D12_RESOURCE_STATES startState)
{
	HRESULT hr = device->CreateCommittedResource(
		&this->heapProperties,
		D3D12_HEAP_FLAG_NONE,
		resourceDesc,
		startState,
		clearValue,
		IID_PPV_ARGS(&this->resource)
	);

	if (FAILED(hr))
	{
		std::string cbName(this->name.begin(), this->name.end());
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create Resource with name: \'%s\'\n", cbName.c_str());
	}

	this->resource->SetName(name.c_str());
}
