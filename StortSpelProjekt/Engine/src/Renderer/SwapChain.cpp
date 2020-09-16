#include "stdafx.h"
#include "SwapChain.h"

#include "GPUMemory/Resource.h"
#include "RenderTarget.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"

SwapChain::SwapChain(
	ID3D12Device5* device,
	const HWND* hwnd,
	unsigned int width, unsigned int height,
	ID3D12CommandQueue* commandQueue,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_Resources[i] = new Resource();
		m_RenderTargets[i] = new RenderTarget(width, height, descriptorHeap_RTV, m_Resources[i]);
	}

	IDXGIFactory4* factory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));

	if (hr != S_OK)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create DXGIFactory for SwapChain\n");
	}

	//Create descriptor
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = width;
	scDesc.Height = height;
	scDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	IDXGISwapChain1* swapChain1 = nullptr;
	if (SUCCEEDED(factory->CreateSwapChainForHwnd(
		commandQueue,
		*hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&swapChain1)))
	{
		if (SUCCEEDED(swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain4))))
		{
			m_pSwapChain4->Release();
		}
	}
	else
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create Swapchain\n");
	}

	SAFE_RELEASE(&factory);

	// Connect the m_RenderTargets to the swapchain, so that the swapchain can easily swap between these two m_RenderTargets
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		HRESULT hr = m_pSwapChain4->GetBuffer(i, IID_PPV_ARGS(m_RenderTargets[i]->GetResource()->GetID3D12Resource1PP()));
		if (FAILED(hr))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to GetBuffer from RenderTarget to Swapchain\n");
		}

		//m_RenderTargets[i]-> = descriptorHeap_RTV->GetNextDescriptorHeapIndex(1);
		D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_RTV->GetCPUHeapAt(m_RenderTargets[i]->GetDescriptorHeapIndex());
		device->CreateRenderTargetView(m_RenderTargets[i]->GetResource()->GetID3D12Resource1(), nullptr, cdh);
	}

	// Create SRVs
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_SRVs[i] = new ShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, &srvDesc, m_Resources[i]);
	}
}

SwapChain::~SwapChain()
{
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		delete m_Resources[i];
		delete m_RenderTargets[i];
		delete m_SRVs[i];
	}
	SAFE_RELEASE(&m_pSwapChain4);
}

IDXGISwapChain4* SwapChain::GetDX12SwapChain() const
{
	return m_pSwapChain4;
}

const RenderTarget* SwapChain::GetRenderTarget(unsigned int backBufferIndex) const
{
	return m_RenderTargets[backBufferIndex];
}

const ShaderResourceView* SwapChain::GetSRV(unsigned int backBufferIndex) const
{
	return m_SRVs[backBufferIndex];
}
