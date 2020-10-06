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
	if (std::atoi(Option::GetInstance().GetVariable("i_windowMode").c_str()) == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		m_WindowMode = static_cast<int>(WINDOW_MODE::FULLSCREEN);
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
	scDesc.Format = m_DesiredColourFormat;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// Microsoft recommends zeroing out the refresh rate of the description before resizing the targets
	m_ZeroRefreshRate.Numerator = 144;
	m_ZeroRefreshRate.Denominator = 1;

	m_ScFullscreenDesc.RefreshRate = m_ZeroRefreshRate;
	m_ScFullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	m_ScFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

	IDXGISwapChain1* swapChain1 = nullptr;
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{	
		scDesc.Scaling = DXGI_SCALING_STRETCH;
		m_ScFullscreenDesc.Windowed = false;
	}
	else
	{
		if (std::atoi(Option::GetInstance().GetVariable("b_stretchedWindow").c_str()) 
			|| (std::atoi(Option::GetInstance().GetVariable("i_windowMode").c_str()) 
				== static_cast<int>(WINDOW_MODE::WINDOWED_FULLSCREEN)))
		{
			scDesc.Scaling = DXGI_SCALING_STRETCH;
		}

		m_ScFullscreenDesc.Windowed = true;
	}

	hr = factory->CreateSwapChainForHwnd(
		commandQueue,
		*hwnd,
		&scDesc,
		&m_ScFullscreenDesc,
		nullptr,
		&swapChain1);

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
	
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		// Finally, activate fullscreen state
		m_pSwapChain4->SetFullscreenState(true, NULL);

		initFullscreen(&width, &height);

		resize(hwnd, width, height);
	}

	createSwapBuffers(device, width, height, descriptorHeap_RTV, descriptorHeap_CBV_UAV_SRV);
}

SwapChain::~SwapChain()
{
	clearSwapBuffers();

	// You can not release the swapchain in fullscreen mode
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
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
	bool fullscreen = false;
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		fullscreen = true;
	}

	return fullscreen;
}

void SwapChain::ToggleWindowMode(ID3D12Device5* device,
	const HWND* hwnd,
	ID3D12CommandQueue* commandQueue,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		m_WindowMode = 0;
	}
	else
	{
		m_WindowMode = 2;
	}

	clearSwapBuffers();

	unsigned int width = std::atoi(Option::GetInstance().GetVariable("i_resolutionWidth").c_str());
	unsigned int height = std::atoi(Option::GetInstance().GetVariable("i_resolutionHeight").c_str());

	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		m_ScDesc.Scaling = DXGI_SCALING_STRETCH;
		m_ScFullscreenDesc.Windowed = false;

		m_pSwapChain4->SetFullscreenState(true, nullptr);

		initFullscreen(&width, &height);

		Option::GetInstance().SetVariable("i_windowMode", std::to_string(2));
	}
	else
	{
		m_ScFullscreenDesc.Windowed = true;
		m_pSwapChain4->SetFullscreenState(false, nullptr);

		// If windowed fullscreen
		if (std::atoi(Option::GetInstance().GetVariable("i_windowMode").c_str()) == static_cast<int>(WINDOW_MODE::WINDOWED_FULLSCREEN))
		{
			m_WindowMode = 1;
			m_ScDesc.Scaling = DXGI_SCALING_STRETCH;
		}
		else
		{
			//m_ScDesc.Scaling = DXGI_SCALING_NONE;
			Option::GetInstance().SetVariable("i_windowMode", std::to_string(0));
		}
	}

	resize(hwnd, width, height);

	createSwapBuffers(device, width, height, descriptorHeap_RTV, descriptorHeap_CBV_UAV_SRV);
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
			rat.Numerator = 144;
			rat.Denominator = 1;

			supportedMode = true;
			m_CurrentModeDescription = m_pSupportedModes[i];
			m_CurrentModeDescription.RefreshRate = rat;
			break;
		}
	}

	// if the current resolution is not supported, switch to the heighest supported resolution
	if (!supportedMode)
	{
		// print a warning 
		Log::PrintSeverity(Log::Severity::WARNING,
			"The desired screen resolution is not supported! Resizing to the lowest supported resolution...\n");

		// set the mode to the lowest supported resolution
		m_CurrentModeDescription = m_pSupportedModes[m_NumberOfSupportedModes-1];
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

	delete[] m_pSupportedModes;
}

const void SwapChain::resize(const HWND* hwnd, unsigned int width, unsigned int height)
{
	// If windowed fullscreen, get screen size
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::WINDOWED_FULLSCREEN))
	{
		HMONITOR hmon = MonitorFromWindow(const_cast<HWND>(*hwnd), MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		m_CurrentModeDescription.Width = mi.rcMonitor.right - mi.rcMonitor.left;
		m_CurrentModeDescription.Height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	if (FAILED(m_pSwapChain4->ResizeTarget(&m_CurrentModeDescription)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to resize target to a supported display mode!");
	}
	else
	{
		m_pSwapChain4->ResizeBuffers(NUM_SWAP_BUFFERS, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}

	// If windowed fullscreen, fit the window to the screen
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::WINDOWED_FULLSCREEN))
	{
		// Make sure that the window starts at the top left corner
		MoveWindow(const_cast<HWND>(*hwnd), 0, 0, m_CurrentModeDescription.Width, m_CurrentModeDescription.Height, true);
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
	srvDesc.Format = m_DesiredColourFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		m_SRVs[i] = new ShaderResourceView(device, descriptorHeap_CBV_UAV_SRV, &srvDesc, m_Resources[i]);
	}
}

void SwapChain::clearSwapBuffers()
{
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		delete m_Resources[i];
		delete m_RTVs[i];
		delete m_SRVs[i];
	}
}