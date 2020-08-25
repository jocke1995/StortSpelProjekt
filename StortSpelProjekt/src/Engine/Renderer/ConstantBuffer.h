#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "Resource.h"
#include "DescriptorHeap.h"

class ConstantBuffer
{
public:
	ConstantBuffer(	ID3D12Device5* device,
					unsigned int entrySize,
					std::wstring resourceName,
					unsigned int descriptorHeapIndex);
	virtual ~ConstantBuffer();

	
	Resource* GetUploadResource() const;
	unsigned int GetDescriptorHeapIndex() const;

protected:
	unsigned int descriptorHeapIndex = -1;

	// will be created with different resources depending on the constantBuffer type
	Resource* uploadResource = nullptr;
	virtual void CreateConstantBufferView(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV) = 0;
private:
	int asd;
};

#endif
