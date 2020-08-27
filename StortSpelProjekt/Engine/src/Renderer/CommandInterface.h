#pragma once
#include "../Headers/stdafx.h"
enum COMMAND_INTERFACE_TYPE
{
	DIRECT_TYPE,
	COPY_TYPE,
	COMPUTE_TYPE
};

class CommandInterface
{
public:
	CommandInterface(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType);
	~CommandInterface();

	ID3D12GraphicsCommandList5* GetCommandList(unsigned int index) const;
	ID3D12CommandAllocator* GetCommandAllocator(unsigned int index) const;

	void Reset(unsigned int index);

private:
	ID3D12GraphicsCommandList5* commandLists[NUM_SWAP_BUFFERS]{ nullptr };
	ID3D12CommandAllocator* commandAllocators[NUM_SWAP_BUFFERS]{ nullptr };

	void CreateCommandInterfaces(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType);
};