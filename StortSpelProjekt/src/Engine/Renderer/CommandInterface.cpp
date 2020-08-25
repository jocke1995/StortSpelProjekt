#include "CommandInterface.h"

CommandInterface::CommandInterface(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType)
{
	this->CreateCommandInterfaces(device, interfaceType);
}

CommandInterface::~CommandInterface()
{
	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		SAFE_RELEASE(&this->commandAllocators[i]);
		SAFE_RELEASE(&this->commandLists[i]);
	}
}

ID3D12GraphicsCommandList5* CommandInterface::GetCommandList(unsigned int index) const
{
	return this->commandLists[index];
}

ID3D12CommandAllocator* CommandInterface::GetCommandAllocator(unsigned int index) const
{
	return this->commandAllocators[index];
}

void CommandInterface::Reset(unsigned int index)
{
	this->commandAllocators[index]->Reset();

	this->commandLists[index]->Reset(this->commandAllocators[index], NULL);
}

void CommandInterface::CreateCommandInterfaces(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType)
{
	D3D12_COMMAND_LIST_TYPE D3D12type;
	switch (interfaceType)
	{
	case COMMAND_INTERFACE_TYPE::DIRECT_TYPE:
		D3D12type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case COMMAND_INTERFACE_TYPE::COPY_TYPE:
		D3D12type = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	case COMMAND_INTERFACE_TYPE::COMPUTE_TYPE:
		D3D12type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	default:
		D3D12type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	}

	for (unsigned int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		HRESULT hr = device->CreateCommandAllocator(D3D12type, IID_PPV_ARGS(&this->commandAllocators[i]));

		if (FAILED(hr))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create CommandAllocator\n");
		}

		hr = device->CreateCommandList(0,
			D3D12type,
			this->commandAllocators[i],
			nullptr,
			IID_PPV_ARGS(&this->commandLists[i]));

		if (FAILED(hr))
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create CommandList\n");
		}

		this->commandLists[i]->Close();
	}
}
