#ifndef CONSTANTBUFFERUPLOAD_H
#define CONSTANTBUFFERUPLOAD_H

#include "ConstantBuffer.h"

class DescriptorHeap;

// DX12 Forward Declarations
struct ID3D12Device5;

class ConstantBufferUpload : public ConstantBufferTemp
{
public:
	ConstantBufferUpload(ID3D12Device5* device,
		unsigned int entrySize,
		std::wstring uploadResourceName,
		unsigned int descriptorHeapIndex,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);

	virtual ~ConstantBufferUpload();


private:

	void createConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV);
};

#endif
