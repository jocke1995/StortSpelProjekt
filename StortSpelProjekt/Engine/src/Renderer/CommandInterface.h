#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include "Core.h"

// DX12 Forward Declarations
struct ID3D12GraphicsCommandList5;
struct ID3D12CommandAllocator;

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
	ID3D12GraphicsCommandList5* m_pCommandLists[NUM_SWAP_BUFFERS]{ nullptr };
	ID3D12CommandAllocator* m_pCommandAllocators[NUM_SWAP_BUFFERS]{ nullptr };

	void createCommandInterfaces(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType);
};

#endif