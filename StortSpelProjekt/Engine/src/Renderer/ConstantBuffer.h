#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "ConstantBufferTemp.h"

static unsigned int cbCounter = 0;
class ConstantBuffer : public ConstantBufferTemp
{
public:
	ConstantBuffer(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring defaultName,
		unsigned int descriptorHeapIndex,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	bool operator == (const ConstantBuffer& other);

	virtual ~ConstantBuffer();

	Resource* GetCBVResource() const;
	
private:
	Resource* m_pDefaultResource = nullptr;
	unsigned int m_pId = 0;

	void CreateConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
};

#endif
