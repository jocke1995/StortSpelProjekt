#ifndef SHADERRESOURCE_H
#define SHADERRESOURCE_H

class Resource;
class DescriptorHeap;
class ShaderResourceView;

static unsigned int s_SrCounter = 0;
class ShaderResource
{
public:
	ShaderResource(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring resourceName,
		D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	bool operator == (const ShaderResource& other);

	virtual ~ShaderResource();

	Resource* const GetUploadResource() const;
	const Resource* const GetDefaultResource() const;

	const ShaderResourceView* const GetSRV() const;

private:
	Resource* m_pUploadResource = nullptr;
	Resource* m_pDefaultResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;

	unsigned int m_pId = 0;
};

#endif
