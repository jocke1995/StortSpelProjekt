#ifndef CONSTANTBUFFERVIEW_H
#define CONSTANTBUFFERVIEW_H

class DescriptorHeap;
class Resource;

class ConstantBufferView
{
public:
	ConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc,
		Resource* resource);

	virtual ~ConstantBufferView();

	unsigned int GetDescriptorHeapIndex() const;
	const Resource* const GetResource() const;

private:
	Resource* m_pResource = nullptr;
	unsigned int m_DescriptorHeapIndex = -1;

	void createConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
};

#endif

