#include "stdafx.h"
#include "SwapChain.h"

#include "Resource.h"
#include "DescriptorHeap.h"

SwapChain::SwapChain(
	ID3D12Device5* device,
	const HWND* hwnd,
	unsigned int width, unsigned int height,
	ID3D12CommandQueue* commandQueue,
	DescriptorHeap* descriptorHeap_RTV,
	unsigned int numRenderTargets)
	:RenderTarget(width, height, numRenderTargets)
{
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
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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

	//D3D12_RENDER_TARGET_VIEW_DESC rtvd = {};
	//rtvd.

	// Connect the m_RenderTargets to the swapchain, so that the swapchain can easily swap between these two m_RenderTargets
	for (unsigned int i = 0; i < numRenderTargets; i++)
	{
		HRESULT hr = m_pSwapChain4->GetBuffer(i, IID_PPV_ARGS(m_Resources[i]->GetID3D12Resource1PP()));
		if (FAILED(hr))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to GetBuffer from RenderTarget to Swapchain\n");
		}

		m_dhIndices[i] = descriptorHeap_RTV->GetNextDescriptorHeapIndex(1);
		D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_RTV->GetCPUHeapAt(m_dhIndices[i]);
		device->CreateRenderTargetView(*m_Resources[i]->GetID3D12Resource1PP(), nullptr, cdh);
	}
}

SwapChain::~SwapChain()
{
	SAFE_RELEASE(&m_pSwapChain4);
}

IDXGISwapChain4* SwapChain::GetDX12SwapChain() const
{
	return m_pSwapChain4;
}
