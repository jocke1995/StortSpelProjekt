#ifndef RENDERTARGET_H
#define RENDERTARGET_H

class Resource;
class DescriptorHeap;
class RenderTargetView;

static unsigned int s_RtCounter = 0;
class RenderTarget
{
public:
	RenderTarget(ID3D12Device5* device,
		unsigned int width, unsigned int height,
		std::wstring resourceName,
		DescriptorHeap* descriptorHeap_RTV);

	bool operator == (const RenderTarget& other);

	virtual ~RenderTarget();

	const Resource* const GetDefaultResource() const;

	const RenderTargetView* const GetRTV() const;

private:
	Resource* m_pResource = nullptr;
	RenderTargetView* m_pRTV = nullptr;

	unsigned int m_Id = 0;

	void createResource(ID3D12Device5* device, unsigned int width, unsigned int height, std::wstring resourceName);
};

#endif
