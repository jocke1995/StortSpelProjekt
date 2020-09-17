#include "stdafx.h"
#include "RenderTargetView.h"

#include "../DescriptorHeap.h"
#include "Resource.h"
#include "../RenderView.h"

RenderTargetView::RenderTargetView(
	ID3D12Device5* device,
	unsigned int width, unsigned int height,
	DescriptorHeap* descriptorHeap_RTV,
	D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
	Resource* resource,
	bool createRTVInConstructor)
	:View(descriptorHeap_RTV, resource)
{
	m_pRenderView = new RenderView(width, height);

	// Swapchain calls this function separatly
	if (createRTVInConstructor == true)
	{
		CreateRTV(device, descriptorHeap_RTV, rtvDesc);
	}
}

RenderTargetView::~RenderTargetView()
{
	delete m_pRenderView;
}

void RenderTargetView::CreateRTV(ID3D12Device5* device, DescriptorHeap* descriptorHeap_RTV, D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_RTV->GetCPUHeapAt(m_DescriptorHeapIndex);
	device->CreateRenderTargetView(m_pResource->GetID3D12Resource1(), rtvDesc, cdh);
}

RenderView* RenderTargetView::GetRenderView() const
{
	return m_pRenderView;
}
