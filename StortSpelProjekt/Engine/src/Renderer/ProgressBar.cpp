#include "stdafx.h"
#include "ProgressBar.h"

#include "GPUMemory/ConstantBuffer.h"
#include "DescriptorHeap.h"

ProgressBar::ProgressBar(
	ID3D12Device5* device,
	unsigned int size,
	std::wstring resourceName,
	DescriptorHeap* dh_CBV_SRV_UAV)
{
	m_Id = s_ProgressBarCounter++;
	m_pConstantBuffer = new ConstantBuffer(device, size, resourceName + std::to_wstring(m_Id), dh_CBV_SRV_UAV);
}

ProgressBar::~ProgressBar()
{
	if (m_pConstantBuffer != nullptr)
	{
		delete m_pConstantBuffer;
	}
}

ConstantBuffer* ProgressBar::GetConstantBuffer() const
{
	return m_pConstantBuffer;
}
