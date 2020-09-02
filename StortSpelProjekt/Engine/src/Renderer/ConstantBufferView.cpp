#include "stdafx.h"
#include "ConstantBufferView.h"

#include "Resource.h"
#include "DescriptorHeap.h"

ConstantBufferView::ConstantBufferView(ID3D12Device5* device, unsigned int entrySize, std::wstring defaultName, unsigned int descriptorHeapIndex, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
	:ConstantBuffer(device, entrySize, L"ConstantBuffer_UPLOAD_RESOURCE", descriptorHeapIndex)
{
	unsigned int sizeAligned = (entrySize + 255) & ~255;
	m_pId = cbvCounter++;
	m_pDefaultResource = new Resource(device, sizeAligned, RESOURCE_TYPE::DEFAULT, defaultName);
	CreateConstantBufferView(device, descriptorHeap_CBV_UAV_SRV);
}

bool ConstantBufferView::operator==(const ConstantBufferView& other)
{
	return m_pId == other.m_pId;
}

ConstantBufferView::~ConstantBufferView()
{
	delete m_pDefaultResource;
}

Resource* ConstantBufferView::GetCBVResource() const
{
	return m_pDefaultResource;
}

void ConstantBufferView::CreateConstantBufferView(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = m_pDefaultResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvd.SizeInBytes = m_pDefaultResource->GetSize();
	device->CreateConstantBufferView(&cbvd, cdh);
}
