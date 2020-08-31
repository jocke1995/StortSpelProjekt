#include "stdafx.h"
#include "CopyPerFrameTask.h"

CopyPerFrameTask::CopyPerFrameTask(ID3D12Device5* device)
	:CopyTask(device)
{

}

CopyPerFrameTask::~CopyPerFrameTask()
{

}

void CopyPerFrameTask::ClearSpecific(const Resource* uploadResource)
{
	unsigned int i = 0;
	// Loop through all copyPerFrame tasks
	for (auto& tuple : m_Upload_Default_Data)
	{
		if (std::get<0>(tuple) == uploadResource)
		{
			// Remove
			m_Upload_Default_Data.erase(m_Upload_Default_Data.begin() + i);
		}
		i++;
	}
}

void CopyPerFrameTask::Clear()
{
	m_Upload_Default_Data.clear();
}

void CopyPerFrameTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);

	this->commandInterface->Reset(this->commandInterfaceIndex);

	for (auto& tuple : m_Upload_Default_Data)
	{
		copyResource(
			commandList,
			std::get<0>(tuple),		// UploadHeap
			std::get<1>(tuple),		// DefaultHeap
			std::get<2>(tuple));	// Data
	}

	commandList->Close();
}
