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
		D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
		Resource* resource);

	virtual ~ShaderResourceView();

	unsigned int GetDescriptorHeapIndex() const;

private:
	unsigned int descriptorHeapIndex = -1;

	void CreateShaderResourceView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
		Resource* resource);
};

#endif
