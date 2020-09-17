#ifndef SHADERRESOURCEVIEW_H
#define SHADERRESOURCEVIEW_H

class DescriptorHeap;
class Resource;

class ShaderResourceView
{
public:
	ShaderResourceView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
		Resource* resource);

	virtual ~ShaderResourceView();

	unsigned int GetDescriptorHeapIndex() const;
	const Resource* const GetResource() const;

private:
	Resource* m_pResource = nullptr;
	unsigned int m_DescriptorHeapIndex = -1;

	void createShaderResourceView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
};

#endif
