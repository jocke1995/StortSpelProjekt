#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
class ShaderResourceView;
class UnorderedAccessView;

#include <array>
class Bloom
{
public:
	Bloom(ID3D12Device5* device, DescriptorHeap* dhRTV, DescriptorHeap* dh_CBV_UAV_SRV, const HWND* hwnd);
	virtual ~Bloom();

	const RenderTarget* const GetRenderTarget() const;

private:
	RenderTarget* m_pRenderTarget = nullptr;
	std::array<ShaderResourceView*, NUM_SWAP_BUFFERS> m_SRVs;
	std::array<UnorderedAccessView*, NUM_SWAP_BUFFERS> m_UAVs;

	void createBrightRenderTarget(ID3D12Device5* device, DescriptorHeap* dhRTV, unsigned int width, unsigned int height);
	void createShaderResourceViews(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV);
	void createUnorderedAccessViews(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV, unsigned int width, unsigned int height);
};

#endif
