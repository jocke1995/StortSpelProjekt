#ifndef CONSTANTBUFFERVIEW_H
#define CONSTANTBUFFERVIEW_H

#include "ConstantBuffer.h"

static unsigned int cbvCounter = 0;
class ConstantBufferView : public ConstantBuffer
{
public:
	ConstantBufferView(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring defaultName,
		unsigned int descriptorHeapIndex,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	bool operator == (const ConstantBufferView& other);

	virtual ~ConstantBufferView();

	Resource* GetCBVResource() const;
	
private:
	Resource* defaultResource = nullptr;
	unsigned int id = 0;

	void CreateConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
};

#endif
