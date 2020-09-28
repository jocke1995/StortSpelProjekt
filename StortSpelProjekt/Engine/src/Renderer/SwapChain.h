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

private:
	std::array<Resource*, NUM_SWAP_BUFFERS> m_Resources;
	std::array<RenderTargetView*, NUM_SWAP_BUFFERS> m_RTVs;
	std::array<ShaderResourceView*, NUM_SWAP_BUFFERS> m_SRVs;
	IDXGISwapChain4* m_pSwapChain4 = nullptr;
	bool m_Fullscreen = false;
};

#endif