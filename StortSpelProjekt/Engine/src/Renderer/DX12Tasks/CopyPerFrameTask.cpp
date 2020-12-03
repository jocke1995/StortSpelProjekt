#include "stdafx.h"
#include "CopyPerFrameTask.h"

#include "../CommandInterface.h"

CopyPerFrameTask::CopyPerFrameTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD)
	:CopyTask(device, interfaceType, FLAG_THREAD)
{

}

CopyPerFrameTask::~CopyPerFrameTask()
{

}

void CopyPerFrameTask::ClearSpecific(const Resource* uploadResource)
{
	unsigned int i = 0;
	// Loop through all copyPerFrame tasks
	for (auto& tuple : m_UploadDefaultData)
	{
		if (std::get<0>(tuple) == uploadResource)
		{
			// Remove
			m_UploadDefaultData.erase(m_UploadDefaultData.begin() + i);
		}
		i++;
	}
}

void CopyPerFrameTask::Clear()
{
	m_UploadDefaultData.clear();
}

void CopyPerFrameTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);



	volatile unsigned int i = 0;	// FOR SOLVING A BIG, REMOVE LATER. TODO
	for (auto& tuple : m_UploadDefaultData)
	{
		if (std::get<2>(tuple) == nullptr)
		{
			Log::PrintSeverity(Log::Severity::CRITICAL, "Data is nullptr\n");
			continue;
		}

		volatile std::vector<DirectX::XMFLOAT4X4>* temp = ((std::vector<DirectX::XMFLOAT4X4>*)(std::get<2>(tuple)));
		volatile std::vector<DirectX::XMFLOAT4X4>* matrices = (reinterpret_cast<std::vector<DirectX::XMFLOAT4X4>*>(const_cast<void*>(std::get<2>(tuple))));
		volatile PointLight* kebab = (PointLight*)(std::get<2>(tuple));

		copyResource(
			commandList,
			std::get<0>(tuple),		// UploadHeap
			std::get<1>(tuple),		// DefaultHeap
			std::get<2>(tuple));	// Data

		i++;						//TODO
	}

	commandList->Close();
}
