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
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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
	
	if (m_Fullscreen)
	{
		initFullscreen(&width, &height);

		resize(width, height);

		// Finally, activate fullscreen state
		m_pSwapChain4->SetFullscreenState(true, NULL);
	}

	createSwapBuffers(device, width, height, descriptorHeap_RTV, descriptorHeap_CBV_UAV_SRV);
}

SwapChain::~SwapChain()
{
	clearSwapBuffers();

	// You can not release the swapchain in fullscreen mode
	if (m_Fullscreen)
	{
		m_pSwapChain4->SetFullscreenState(false, NULL);
	}

	delete[] m_pSupportedModes;

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

void SwapChain::Toggle(ID3D12Device5* device,
	const HWND* hwnd,
	ID3D12CommandQueue* commandQueue,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_Fullscreen = std::atoi(Option::GetInstance().GetVariable("b_fullscreen").c_str());

	clearSwapBuffers();

	unsigned int width = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	unsigned int height = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());

	if (m_Fullscreen)
	{
		initFullscreen(&width, &height);

		m_pSwapChain4->SetFullscreenState(true, nullptr);
	}
	else
	{
		m_pSwapChain4->SetFullscreenState(false, nullptr);
	}

	resize(width, height);

	createSwapBuffers(device, width, height, descriptorHeap_RTV, descriptorHeap_CBV_UAV_SRV);

	Option::GetInstance().SetVariable("b_fullscreen", "0");
}

const void SwapChain::initFullscreen(unsigned int* width, unsigned int* height)
{
	// enumerate all available display modes
	// get representation of the output adapter
	IDXGIOutput* output = nullptr;
	if (FAILED(m_pSwapChain4->GetContainingOutput(&output)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to retrieve the output adapter!\n");
	}

	// get the amount of supported display modes for the desired format
	if (FAILED(output->GetDisplayModeList(m_DesiredColourFormat, 0, &m_NumberOfSupportedModes, NULL)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to list all supported display modes!\n");
	}

	// set up array for the supported modes
	m_pSupportedModes = new DXGI_MODE_DESC[m_NumberOfSupportedModes];
	ZeroMemory(m_pSupportedModes, sizeof(DXGI_MODE_DESC) * m_NumberOfSupportedModes);

	// fill the array with the available display modes
	if (FAILED(output->GetDisplayModeList(m_DesiredColourFormat, 0, &m_NumberOfSupportedModes, m_pSupportedModes)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to retrieve all supported display modes!\n");
	}

	// release the output adapter
	output->Release();

	bool supportedMode = false;
	for (unsigned int i = 0; i < m_NumberOfSupportedModes; i++)
	{
		if (*width == m_pSupportedModes[i].Width && *height == m_pSupportedModes[i].Height)
		{
			DXGI_RATIONAL rat = {};
			rat.Numerator = 0;
			rat.Denominator = 0;

			supportedMode = true;
			m_CurrentModeDescription = m_pSupportedModes[i];
			m_CurrentModeDescription.RefreshRate = rat;
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
		m_CurrentModeDescription = m_pSupportedModes[0];
		*width = m_CurrentModeDescription.Width;
		*height = m_CurrentModeDescription.Height;

		Log::Print("Supported resolutions:\n");
		int latestWidth = 0;
		int latestHeight = 0;
		for (unsigned int i = 0; i < m_NumberOfSupportedModes; i++)
		{
			// We don't care about other attributes, only the resolution
			if (m_pSupportedModes[i].Width == latestWidth && m_pSupportedModes[i].Height == latestHeight)
			{
				continue;
			}
			Log::Print("(%d, %d)\n", m_pSupportedModes[i].Width, m_pSupportedModes[i].Height);
			latestWidth = m_pSupportedModes[i].Width;
			latestHeight = m_pSupportedModes[i].Height;
		}
		Log::Print("----------------------\n");
	}
}

const void SwapChain::resize(unsigned int width, unsigned int height)
{
	if (FAILED(m_pSwapChain4->ResizeTarget(&m_CurrentModeDescription)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to resize target to a supported display mode!");
	}
	else
	{
		m_pSwapChain4->ResizeBuffers(NUM_SWAP_BUFFERS, width, height, m_DesiredColourFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}
}

const void SwapChain::createSwapBuffers(ID3D12Device5* device,
	unsigned int width, unsigned int height,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
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

const void SwapChain::clearSwapBuffers() const
{
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		delete m_Resources[i];
		delete m_RTVs[i];
		delete m_SRVs[i];
	}
}
