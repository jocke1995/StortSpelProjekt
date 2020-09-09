#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
//class ShaderResourceView;
//class UnorderedAccessView;
class Bloom
{
public:
	Bloom(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd);
	virtual ~Bloom();

	const RenderTarget* const GetRenderTarget() const;
private:
	RenderTarget* m_pRenderTarget;


	void createBrightRenderTarget(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd);
};

#endif
