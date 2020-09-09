#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
class ShaderResourceView;

#include <array>
//class UnorderedAccessView;
class Bloom
{
public:
	Bloom(ID3D12Device5* device, DescriptorHeap* dhRTV, DescriptorHeap* dh_CBV_UAV_SRV, const HWND* hwnd);
	virtual ~Bloom();

	const RenderTarget* const GetRenderTarget() const;

private:
	RenderTarget* m_pRenderTarget = nullptr;
	std::array<ShaderResourceView*, NUM_SWAP_BUFFERS> m_SRVs;

	void createBrightRenderTarget(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd);
	void createShaderResourceView(ID3D12Device5* device, DescriptorHeap* dh_CBV_UAV_SRV);
};

#endif
