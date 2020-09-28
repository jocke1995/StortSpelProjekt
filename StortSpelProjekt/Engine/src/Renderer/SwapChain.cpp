#include "stdafx.h"
#include "SwapChain.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/RenderTargetView.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"
#include "../Misc/Option.h"

SwapChain::SwapChain(
	ID3D12Device5* device,
	const HWND* hwnd,
	unsigned int width, unsigned int height,
	ID3D12CommandQueue* commandQueue,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_Fullscreen = std::atoi(Option::GetInstance().GetVariable("b_fullscreen").c_str());

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
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.Flags = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	IDXGISwapChain1* swapChain1 = nullptr;
	if (m_Fullscreen)
	{	
		scDesc.Scaling = DXGI_SCALING_STRETCH;

		// Microsoft recommends zeroing out the refresh rate of the description before resizing the targets
		DXGI_RATIONAL zeroRefreshRate = {};
		zeroRefreshRate.Numerator = 0;
		zeroRefreshRate.Denominator = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC scFullscreenDesc = {};
		scFullscreenDesc.RefreshRate = zeroRefreshRate;
		scFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		scFullscreenDesc.Windowed = false;

		hr = factory->CreateSwapChainForHwnd(
			commandQueue,
			*hwnd,
			&scDesc,
			&scFullscreenDesc,
			nullptr,
			&swapChain1);
	}
	else
	{
		if (std::atoi(Option::GetInstance().GetVariable("b_stretchedWindow").c_str()) 
			|| std::atoi(Option::GetInstance().GetVariable("b_windowedFullscreen").c_str()))
		{
			scDesc.Scaling = DXGI_SCALING_STRETCH;
		}

		hr = factory->CreateSwapChainForHwnd(
			commandQueue,
			*hwnd,
			&scDesc,
			nullptr,
			nullptr,
			&swapChain1);
	}

	if (SUCCEEDED(hr))
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
	
	// If fullscreen, find if the current resolution is supported and get the width and height for the RTV
	if (m_Fullscreen)
	{
		// colour format
		DXGI_FORMAT desiredColourFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		unsigned int numberOfSupportedModes = 0;	// the number of supported screen modes for the desired colour format
		DXGI_MODE_DESC* supportedModes = {};		// list of all supported screen modes for the desired colour format
		DXGI_MODE_DESC currentModeDescription = {};	// description of the currently active screen mode

		// enumerate all available display modes
		// get representation of the output adapter
		IDXGIOutput* output = nullptr;
		if (FAILED(swapChain1->GetContainingOutput(&output)))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to retrieve the output adapter!\n");
		}

		// get the amount of supported display modes for the desired format
		if (FAILED(output->GetDisplayModeList(desiredColourFormat, 0, &numberOfSupportedModes, NULL)))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to list all supported display modes!\n");
		}

		// set up array for the supported modes
		supportedModes = new DXGI_MODE_DESC[numberOfSupportedModes];
		ZeroMemory(supportedModes, sizeof(DXGI_MODE_DESC) * numberOfSupportedModes);

		// fill the array with the available display modes
		if (FAILED(output->GetDisplayModeList(desiredColourFormat, 0, &numberOfSupportedModes, supportedModes)))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to retrieve all supported display modes!\n");
		}

		// release the output adapter
		output->Release();

		bool supportedMode = false;
		for (unsigned int i = 0; i < numberOfSupportedModes; i++)
		{
			if (width == supportedModes[i].Width && height == supportedModes[i].Height)
			{
				supportedMode = true;
				currentModeDescription = supportedModes[i];
				break;
			}
		}

		// if the current resolution is not supported, switch to the lowest supported resolution
		if (!supportedMode)
		{
			// print a warning 
			Log::PrintSeverity(Log::Severity::WARNING,
				"The desired screen resolution is not supported! Resizing to the lowest supported resolution...\n");

			// set the mode to the lowest supported resolution
			currentModeDescription = supportedModes[0];
			width = currentModeDescription.Width;
			height = currentModeDescription.Height;

			Log::Print("Supported resolutions:\n");
			int latestWidth = 0;
			int latestHeight = 0;
			for (unsigned int i = 0; i < numberOfSupportedModes; i++)
			{
				// We don't care about other attributes, only the resolution
				if (supportedModes[i].Width == latestWidth && supportedModes[i].Height == latestHeight)
				{
					continue;
				}
				Log::Print("(%d, %d)\n", supportedModes[i].Width, supportedModes[i].Height);
				latestWidth = supportedModes[i].Width;
				latestHeight = supportedModes[i].Height;
			}
			Log::Print("----------------------\n");
		}

		if (FAILED(m_pSwapChain4->ResizeTarget(&currentModeDescription)))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to resize target to a supported display mode!");
		}
		else
		{
			m_pSwapChain4->ResizeBuffers(NUM_SWAP_BUFFERS, width, height, desiredColourFormat, 0);
		}

		delete[] supportedModes;

		// Finally, activate fullscreen state
		m_pSwapChain4->SetFullscreenState(true, NULL);
	}

	// Set RTVs
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_Resources[i] = new Resource();
		m_RTVs[i] = new RenderTargetView(device, width, height, descriptorHeap_RTV, nullptr, m_Resources[i], false);
	}

	// Connect the m_RenderTargets to the swapchain, so that the swapchain can easily swap between these two m_RenderTargets
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		HRESULT hr = m_pSwapChain4->GetBuffer(i, IID_PPV_ARGS(const_cast<Resource*>(m_RTVs[i]->GetResource())->GetID3D12Resource1PP()));
		if (FAILED(hr))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to GetBuffer from RenderTarget to Swapchain\n");
		}

		m_RTVs[i]->CreateRTV(device, descriptorHeap_RTV, nullptr);
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
		delete m_RTVs[i];
		delete m_SRVs[i];
	}

	// You can not release the swapchain in fullscreen mode
	if (m_Fullscreen)
	{
		m_pSwapChain4->SetFullscreenState(false, NULL);
	}

	SAFE_RELEASE(&m_pSwapChain4);
}

IDXGISwapChain4* SwapChain::GetDX12SwapChain() const
{
	return m_pSwapChain4;
}

const RenderTargetView* SwapChain::GetRTV(unsigned int backBufferIndex) const
{
	return m_RTVs[backBufferIndex];
}

const ShaderResourceView* SwapChain::GetSRV(unsigned int backBufferIndex) const
{
	return m_SRVs[backBufferIndex];
}

const bool SwapChain::IsFullscreen() const
{
	return m_Fullscreen;
}
