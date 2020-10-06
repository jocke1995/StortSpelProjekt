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

	HMONITOR hmon = MonitorFromWindow(const_cast<HWND>(*hwnd), MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hmon, &mi);

	int m_ScreenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	int m_ScreenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

	// If the chosen resolution is higher than the screen resolution, set the resolution to the screen resolution
	if (m_ScreenWidth < width)
	{
		m_CurrentModeDescription.Width = m_ScreenWidth;
		m_CurrentModeDescription.Height = m_ScreenHeight;
		Log::Print("Choosing (%d, %d) as the new resolution...\n", m_CurrentModeDescription.Width, m_CurrentModeDescription.Height);
	}
	else
	{
		m_CurrentModeDescription.Width = width;
		m_CurrentModeDescription.Height = height;
	}

	//Create descriptor
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = m_CurrentModeDescription.Width;
	scDesc.Height = m_CurrentModeDescription.Height;
	scDesc.Format = m_DesiredColourFormat;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	m_RefreshRate.Numerator = 200;
	m_RefreshRate.Denominator = 1;

	m_CurrentModeDescription.Width = width;
	m_CurrentModeDescription.Height = height;
	m_CurrentModeDescription.RefreshRate = m_RefreshRate;

	IDXGISwapChain1* swapChain1 = nullptr;
	hr = factory->CreateSwapChainForHwnd(
		commandQueue,
		*hwnd,
		&scDesc,
		nullptr,
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

		resize(hwnd);
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

	if (m_WindowMode == static_cast<int>(WINDOW_MODE::FULLSCREEN))
	{
		m_pSwapChain4->SetFullscreenState(true, nullptr);

		Option::GetInstance().SetVariable("i_windowMode", std::to_string(2));
	}
	else
	{
		m_pSwapChain4->SetFullscreenState(false, nullptr);

		Option::GetInstance().SetVariable("i_windowMode", std::to_string(0));
	}

	resize(hwnd);

	createSwapBuffers(device, m_CurrentModeDescription.Width, m_CurrentModeDescription.Height, descriptorHeap_RTV, descriptorHeap_CBV_UAV_SRV);
}

const void SwapChain::lookForSupportedResolutions(unsigned int* width, unsigned int* height)
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

	// if the current resolution is not supported, switch to the nearest supported resolution
	if (!supportedMode)
	{
		// print a warning 
		Log::PrintSeverity(Log::Severity::WARNING,
			"The desired screen resolution is not supported! Resizing to the nearest supported resolution...\n");

		for (unsigned int i = 0; i < m_NumberOfSupportedModes; i++)
		{
			// We don't care about other attributes, only the resolution
			if (m_pSupportedModes[i].Width > static_cast<UINT>(*width))
			{
				// set the mode to the nearest supported resolution
				*width = m_pSupportedModes[i].Width;
				*height = m_pSupportedModes[i].Height;
				break;
			}
		}

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

const void SwapChain::resize(const HWND* hwnd)
{
	if (FAILED(m_pSwapChain4->ResizeTarget(&m_CurrentModeDescription)))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Unable to resize target to a supported display mode!");
	}
	else
	{
		m_pSwapChain4->ResizeBuffers(NUM_SWAP_BUFFERS, 
			m_CurrentModeDescription.Width, m_CurrentModeDescription.Height,
			DXGI_FORMAT_UNKNOWN,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}

	// If not fullscreen, fit the window to the screen
	if (m_WindowMode == static_cast<int>(WINDOW_MODE::WINDOWED))
	{
		// Make sure that the window starts at the top left corner
		int windowWidth = std::atoi(Option::GetInstance().GetVariable("i_windowWidth").c_str());
		int windowHeight = std::atoi(Option::GetInstance().GetVariable("i_windowHeight").c_str());
		SetWindowPos(const_cast<HWND>(*hwnd), 0, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		MoveWindow(const_cast<HWND>(*hwnd), 0, 0, windowWidth, windowHeight, false);
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