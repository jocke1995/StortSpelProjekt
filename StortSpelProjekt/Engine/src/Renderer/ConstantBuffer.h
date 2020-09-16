#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

class Resource;
class DescriptorHeap;

static unsigned int cbCounter = 0;
class ConstantBuffer
{
public:
	ConstantBuffer(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring resourceName,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	bool operator == (const ConstantBuffer& other);

	virtual ~ConstantBuffer();

	Resource* GetUploadResource() const;
	Resource* GetDefaultResource() const;

	unsigned int GetDescriptorHeapIndex() const;
	
private:
	Resource* m_pUploadResource = nullptr;
	Resource* m_pDefaultResource = nullptr;
	unsigned int m_pId = 0;

	unsigned int m_DescriptorHeapIndex = 0;

	void CreateConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
};

#endif
