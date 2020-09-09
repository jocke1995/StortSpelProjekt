#ifndef UNORDEREDACCESSVIEW_H
#define UNORDEREDACCESSVIEW_H

class DescriptorHeap;
class Resource;

class UnorderedAccessView
{
public:
	UnorderedAccessView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
		unsigned int width, unsigned int height);
	virtual ~UnorderedAccessView();

	unsigned int GetDescriptorHeapIndex() const;
private:
	unsigned int m_DescriptorHeapIndex = -1;

	Resource* m_pResource = nullptr;

	void createUnorderedAccessView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);
};

#endif