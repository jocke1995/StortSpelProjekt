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
	m_Data_CBVs.clear();
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

void CopyOnDemandTask::copyTexture(Texture* texture)
{

}
