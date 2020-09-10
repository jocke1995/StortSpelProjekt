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
	BloomResources(ID3D12Device5* device, DescriptorHeap* dhRTV, DescriptorHeap* dh_CBV_UAV_SRV, const HWND* hwnd);
	virtual ~BloomResources();

	const RenderTarget* const GetRenderTarget() const;
	const Resource* const GetResourceToWrite(unsigned int index) const;

private:
	RenderTarget* m_pRenderTarget = nullptr;
	std::array<Resource*, NUM_SWAP_BUFFERS> m_ResourcesToWrite;

	void createBrightRenderTarget(ID3D12Device5* device, DescriptorHeap* dhRTV, unsigned int width, unsigned int height);
};

#endif
