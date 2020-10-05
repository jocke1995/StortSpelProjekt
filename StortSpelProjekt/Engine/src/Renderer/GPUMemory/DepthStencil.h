#ifndef DEPTHSTENCIL_H
#define DEPTHSTENCIL_H

class Resource;
class DescriptorHeap;
class DepthStencilView;

static unsigned int s_DsCounter = 0;
class DepthStencil
{
public:
	DepthStencil(
		ID3D12Device5* device,
		unsigned int width,
		unsigned int height,
		std::wstring resourceName,
		D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
		DescriptorHeap* descriptorHeap_DSV);

	bool operator == (const DepthStencil& other);

	virtual ~DepthStencil();

	const Resource* const GetDefaultResource() const;
	const DepthStencilView* const GetDSV() const;

private:
	Resource* m_pDefaultResource = nullptr;
	DepthStencilView* m_pDSV = nullptr;

	unsigned int m_Id = 0;
	void createResource(
		ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring resourceName,
		DXGI_FORMAT format);
};

#endif
