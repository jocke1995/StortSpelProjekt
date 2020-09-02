#include "stdafx.h"
#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device5* device, DESCRIPTOR_HEAP_TYPE type)
{
	// Create description
	switch (type)
	{
	case DESCRIPTOR_HEAP_TYPE::RTV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		m_Desc.NumDescriptors = 10;
		break;
	case DESCRIPTOR_HEAP_TYPE::DSV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		m_Desc.NumDescriptors = 10;
		break;
	case DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV:
		m_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		m_Desc.NumDescriptors = 100000;
		m_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	}

	m_HandleIncrementSize = device->GetDescriptorHandleIncrementSize(m_Desc.Type);

	// Create descriptorHeap for the renderTarget
	
	HRESULT hr = device->CreateDescriptorHeap(&m_Desc, IID_PPV_ARGS(&m_pDescriptorHeap));
	if (hr != S_OK)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create DescriptorHeap\n");
	}

	SetCPUGPUHeapStart();
}

DescriptorHeap::~DescriptorHeap()
{
	SAFE_RELEASE(&m_pDescriptorHeap);
}

void DescriptorHeap::SetCPUGPUHeapStart()
{
	m_CPUHeapStart = m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_GPUHeapStart = m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void DescriptorHeap::IncrementDescriptorHeapIndex()
{
	m_DescriptorHeapIndex++;
}

unsigned int DescriptorHeap::GetNextDescriptorHeapIndex(unsigned int increment)
{
	unsigned int indexToReturn = m_DescriptorHeapIndex;

	m_DescriptorHeapIndex += increment;

	return indexToReturn;
}

const D3D12_DESCRIPTOR_HEAP_DESC* DescriptorHeap::GetDesc() const
{
	return &m_Desc;
}

ID3D12DescriptorHeap* DescriptorHeap::GetID3D12DescriptorHeap() const
{
	return m_pDescriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHeapAt(UINT descriptorIndex)
{
	m_CPUHeapAt.ptr = m_CPUHeapStart.ptr + m_HandleIncrementSize * descriptorIndex;
	return m_CPUHeapAt;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHeapAt(UINT descriptorIndex)
{
	m_GPUHeapAt.ptr = m_GPUHeapStart.ptr + m_HandleIncrementSize * descriptorIndex;

	return m_GPUHeapAt;
}

const UINT DescriptorHeap::GetHandleIncrementSize() const
{
	return m_HandleIncrementSize;
}
