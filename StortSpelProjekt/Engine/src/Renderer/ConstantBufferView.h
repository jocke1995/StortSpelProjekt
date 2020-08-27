#ifndef CONSTANTBUFFERVIEW_H
#define CONSTANTBUFFERVIEW_H

#include "ConstantBuffer.h"

class ConstantBufferView : public ConstantBuffer
{
public:
	ConstantBufferView(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring defaultName,
		unsigned int descriptorHeapIndex,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	virtual ~ConstantBufferView();

	Resource* GetCBVResource() const;
	
private:
	Resource* defaultResource = nullptr;

	void CreateConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
};

#endif
