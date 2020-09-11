#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
class Resource;

#include <array>
class BloomResources
{
public:
	BloomResources(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd);
	virtual ~BloomResources();

	const RenderTarget* const GetRenderTarget() const;
	const Resource* const GetResourceToWrite() const;

private:
	// The compute shader will read from the renderTarget and then write "blurred-data" to m_pResourceToWrite
	RenderTarget* m_pRenderTarget = nullptr;
	Resource* m_pResourceToWrite = nullptr;

	void createBrightRenderTarget(ID3D12Device5* device, DescriptorHeap* dhRTV, unsigned int width, unsigned int height);
};

#endif
