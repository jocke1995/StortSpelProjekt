#ifndef UNORDEREDACCESS_H
#define UNORDEREDACCESS_H

class Resource;
class DescriptorHeap;
class UnorderedAccessView;

static unsigned int s_UaCounter = 0;
class UnorderedAccess
{
public:
	UnorderedAccess(
		ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring resourceName,
		D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	bool operator == (const UnorderedAccess& other);

	virtual ~UnorderedAccess();

	Resource* const GetUploadResource() const;
	const Resource* const GetDefaultResource() const;

	const UnorderedAccessView* const GetUAV() const;

private:
	Resource* m_pUploadResource = nullptr;
	Resource* m_pDefaultResource = nullptr;
	UnorderedAccessView* m_pUAV = nullptr;

	unsigned int m_Id = 0;
};

#endif
