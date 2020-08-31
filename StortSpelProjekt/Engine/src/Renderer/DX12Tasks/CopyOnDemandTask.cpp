#include "stdafx.h"
#include "CopyOnDemandTask.h"

CopyOnDemandTask::CopyOnDemandTask(ID3D12Device5* device)
	:CopyTask(device)
{

}

CopyOnDemandTask::~CopyOnDemandTask()
{
}

void CopyOnDemandTask::Clear()
{
	m_Upload_Default_Data.clear();
	m_Textures.clear();
}

void CopyOnDemandTask::SubmitTexture(Texture* texture)
{
	m_Textures.push_back(texture);
}

void CopyOnDemandTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);
	
	this->commandInterface->Reset(this->commandInterfaceIndex);
	
	// Upload the "small" data, such as constantbuffers..
	for (auto& tuple : m_Upload_Default_Data)
	{
		copyResource(
			commandList,
			std::get<0>(tuple),		// UploadHeap
			std::get<1>(tuple),		// DefaultHeap
			std::get<2>(tuple));	// Data
	}

	// Upload "big" texturedata
	
	commandList->Close();
}

void CopyOnDemandTask::copyTexture(Texture* texture)
{

}
