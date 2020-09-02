#include "stdafx.h"
#include "DX12Task.h"

#include "../CommandInterface.h"

DX12Task::DX12Task(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType)
{
	m_pCommandInterface = new CommandInterface(device, interfaceType);
}

DX12Task::~DX12Task()
{
	delete m_pCommandInterface;
}

void DX12Task::SetBackBufferIndex(int backBufferIndex)
{
	m_BackBufferIndex = backBufferIndex;
}

void DX12Task::SetCommandInterfaceIndex(int index)
{
	m_CommandInterfaceIndex = index;
}

ID3D12GraphicsCommandList5* DX12Task::GetCommandList(unsigned int index) const
{
	return m_pCommandInterface->GetCommandList(index);
}
