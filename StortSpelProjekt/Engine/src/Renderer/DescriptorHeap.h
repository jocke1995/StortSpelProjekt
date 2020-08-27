#ifndef DESCRIPTORHEAP_H
#define DESCRIPTORHEAP_H

enum class DESCRIPTOR_HEAP_TYPE
{
	CBV_UAV_SRV,
	RTV,
	DSV,
	NUM_DESCRIPTORHEAP_TYPES
};

class DescriptorHeap
{
public:
	DescriptorHeap(ID3D12Device5* device, DESCRIPTOR_HEAP_TYPE type);
	~DescriptorHeap();

	void SetCPUGPUHeapStart();
	void IncrementDescriptorHeapIndex();

	unsigned int GetNextDescriptorHeapIndex(unsigned int increment = 0);
	const D3D12_DESCRIPTOR_HEAP_DESC* GetDesc() const;
	ID3D12DescriptorHeap* GetID3D12DescriptorHeap() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHeapAt(UINT descriptorIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHeapAt(UINT descriptorIndex);
	const UINT GetHandleIncrementSize() const;

private:
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	unsigned int descriptorHeapIndex = 0;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHeapStart;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHeapAt;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHeapAt;


	UINT handleIncrementSize;
};

#endif