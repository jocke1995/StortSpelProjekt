#include "stdafx.h"
#include "ConstantBufferUpload.h"

#include "DescriptorHeap.h"
#include "Resource.h"

ConstantBufferUpload::ConstantBufferUpload(ID3D12Device5* device, unsigned int entrySize, std::wstring uploadResourceName, unsigned int descriptorHeapIndex, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
	:ConstantBuffer(device, entrySize, uploadResourceName, descriptorHeapIndex)
{
	createConstantBufferView(device, descriptorHeap_CBV_UAV_SRV);
}

ConstantBufferUpload::~ConstantBufferUpload()
{
}

void ConstantBufferUpload::createConstantBufferView(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_CBV_UAV_SRV->GetCPUHeapAt(m_DescriptorHeapIndex);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
	cbvd.BufferLocation = m_pUploadResource->GetID3D12Resource1()->GetGPUVirtualAddress();
	cbvd.SizeInBytes = m_pUploadResource->GetSize();
	device->CreateConstantBufferView(&cbvd, cdh);
}
