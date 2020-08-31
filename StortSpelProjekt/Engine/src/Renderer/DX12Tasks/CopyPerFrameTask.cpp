#include "stdafx.h"
#include "CopyPerFrameTask.h"

CopyPerFrameTask::CopyPerFrameTask(ID3D12Device5* device)
	:CopyTask(device)
{

}

CopyPerFrameTask::~CopyPerFrameTask()
{

}

void CopyPerFrameTask::ClearSpecific(const ConstantBufferView* cbv)
{
	unsigned int i = 0;
	// Loop through all copyPerFrame tasks
	for (auto& pair : m_Data_CBVs)
	{
		if (pair.second == cbv)
		{
			// Remove
			m_Data_CBVs.erase(m_Data_CBVs.begin() + i);
		}
		i++;
	}
}

void CopyPerFrameTask::Clear()
{
	m_Data_CBVs.clear();
}

void CopyPerFrameTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);

	this->commandInterface->Reset(this->commandInterfaceIndex);

	for (auto& pair : m_Data_CBVs)
	{
		copyResource(
			commandList,
			pair.second->GetUploadResource(),
			pair.second->GetCBVResource(),
			pair.first);	// Data
	}

	commandList->Close();
}
