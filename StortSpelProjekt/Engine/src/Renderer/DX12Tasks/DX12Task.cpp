#include "stdafx.h"
#include "DX12Task.h"

#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../GPUMemory/Resource.h"

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

void DX12Task::SetDescriptorHeaps(std::map<DESCRIPTOR_HEAP_TYPE, DescriptorHeap*> dhs)
{
	m_DescriptorHeaps = dhs;
}

void DX12Task::AddResource(std::string id, const Resource* resource)
{
	if (m_Resources[id] == nullptr)
	{
		m_Resources[id] = resource;
	}
	else
	{
		Log::PrintSeverity(
			Log::Severity::CRITICAL,
			"Trying to add Resource with name: \'%s\' that already exists.\n", id);
	}
}

ID3D12GraphicsCommandList5* DX12Task::GetCommandList(unsigned int index) const
{
	return m_pCommandInterface->GetCommandList(index);
}

void DX12Task::TransResourceState(ID3D12GraphicsCommandList5* cl, Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		resource->GetID3D12Resource1(),
		stateBefore,
		stateAfter));
}
