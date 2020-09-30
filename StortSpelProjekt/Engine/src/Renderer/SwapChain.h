#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

class Resource;
class DescriptorHeap;
struct IDXGISwapChain4;

class RenderTargetView;
class ShaderResourceView;

// DX12 Forward Declarations
struct ID3D12Device5;
struct ID3D12CommandQueue;
struct IDXGISwapChain4;

class SwapChain
{
public:
	SwapChain(
		ID3D12Device5* device,
		const HWND* hwnd,
		unsigned int width, unsigned int height,
		ID3D12CommandQueue* commandQueue,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
	virtual ~SwapChain();

	IDXGISwapChain4* GetDX12SwapChain() const;
	const RenderTargetView* GetRTV(unsigned int backBufferIndex) const;
	const ShaderResourceView* GetSRV(unsigned int backBufferIndex) const;
	const bool IsFullscreen() const;
	void ToggleWindowMode(ID3D12Device5* device,
		const HWND* hwnd,
		ID3D12CommandQueue* commandQueue,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

private:
	std::array<Resource*, NUM_SWAP_BUFFERS> m_Resources;
	std::array<RenderTargetView*, NUM_SWAP_BUFFERS> m_RTVs;
	std::array<ShaderResourceView*, NUM_SWAP_BUFFERS> m_SRVs;
	IDXGISwapChain4* m_pSwapChain4 = nullptr;
	DXGI_SWAP_CHAIN_DESC1 scDesc;
	bool m_Fullscreen = false;

	// colour format
	DXGI_FORMAT m_DesiredColourFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	unsigned int m_NumberOfSupportedModes = 0;	// the number of supported screen modes for the desired colour format
	DXGI_MODE_DESC* m_pSupportedModes = {};		// list of all supported screen modes for the desired colour format
	DXGI_MODE_DESC m_CurrentModeDescription = {};	// description of the currently active screen mode

	const void initFullscreen(unsigned int* width, unsigned int* height);
	const void resize(unsigned int width, unsigned int height);
	const void createSwapBuffers(ID3D12Device5* device,
		unsigned int width, unsigned int height,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
	const void clearSwapBuffers() const;
};

#endif