#include "stdafx.h"
#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device5* device, DESCRIPTOR_HEAP_TYPE type)
{
	// Create description
	switch (type)
	{
	case DESCRIPTOR_HEAP_TYPE::RTV:
		this->m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		this->m_Desc.NumDescriptors = 10;
		break;
	case DESCRIPTOR_HEAP_TYPE::DSV:
		this->m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		this->m_Desc.NumDescriptors = 10;
		break;
	case DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV:
		this->m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		this->m_Desc.NumDescriptors = 100000;
		this->m_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	}

	this->m_HandleIncrementSize = device->GetDescriptorHandleIncrementSize(this->m_Desc.Type);

	// Create descriptorHeap for the renderTarget
	
	HRESULT hr = device->CreateDescriptorHeap(&this->m_Desc, IID_PPV_ARGS(&this->m_pDescriptorHeap));
	if (hr != S_OK)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create DescriptorHeap\n");
	}

	this->SetCPUGPUHeapStart();
}

DescriptorHeap::~DescriptorHeap()
{
	SAFE_RELEASE(&this->m_pDescriptorHeap);
}

void DescriptorHeap::SetCPUGPUHeapStart()
{
	this->m_CPUHeapStart = this->m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	this->m_GPUHeapStart = this->m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void DescriptorHeap::IncrementDescriptorHeapIndex()
{
	this->m_DescriptorHeapIndex++;
}

unsigned int DescriptorHeap::GetNextDescriptorHeapIndex(unsigned int increment)
{
	unsigned int indexToReturn = this->m_DescriptorHeapIndex;

	this->m_DescriptorHeapIndex += increment;

	return indexToReturn;
}

const D3D12_DESCRIPTOR_HEAP_DESC* DescriptorHeap::GetDesc() const
{
	return &this->m_Desc;
}

ID3D12DescriptorHeap* DescriptorHeap::GetID3D12DescriptorHeap() const
{
	return this->m_pDescriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHeapAt(UINT descriptorIndex)
{
	this->m_CPUHeapAt.ptr = this->m_CPUHeapStart.ptr + this->m_HandleIncrementSize * descriptorIndex;
	return this->m_CPUHeapAt;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHeapAt(UINT descriptorIndex)
{
	this->m_GPUHeapAt.ptr = this->m_GPUHeapStart.ptr + this->m_HandleIncrementSize * descriptorIndex;

	return this->m_GPUHeapAt;
}

const UINT DescriptorHeap::GetHandleIncrementSize() const
{
	return this->m_HandleIncrementSize;
}
