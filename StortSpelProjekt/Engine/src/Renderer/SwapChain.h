#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "RenderTarget.h"

class Resource;
class DescriptorHeap;
struct IDXGISwapChain4;

// DX12 Forward Declarations
struct ID3D12Device5;
struct ID3D12CommandQueue;
struct IDXGISwapChain4;

class SwapChain : public RenderTarget
{
public:
	SwapChain(
		ID3D12Device5* device,
		const HWND* hwnd,
		unsigned int width, unsigned int height,
		ID3D12CommandQueue* commandQueue,
		DescriptorHeap* descriptorHeap_RTV);
	~SwapChain();

	IDXGISwapChain4* GetDX12SwapChain() const;

private:
	IDXGISwapChain4* swapChain4 = nullptr;
};

#endif