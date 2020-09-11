#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

class Resource;
class DescriptorHeap;
class RenderTarget;
struct IDXGISwapChain4;

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
		DescriptorHeap* descriptorHeap_RTV);
	virtual ~SwapChain();

	IDXGISwapChain4* GetDX12SwapChain() const;
	const RenderTarget* GetRenderTarget(unsigned int backBufferIndex) const;

private:
	std::array<Resource*, NUM_SWAP_BUFFERS> m_Resources;
	std::array<RenderTarget*, NUM_SWAP_BUFFERS> m_RenderTargets;
	IDXGISwapChain4* m_pSwapChain4 = nullptr;
};

#endif