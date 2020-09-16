#include "stdafx.h"
#include "CopyTask.h"

#include "../Resource.h"
#include "../CommandInterface.h"

CopyTask::CopyTask(ID3D12Device5* device)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::COPY_TYPE)
{

}

CopyTask::~CopyTask()
{

}

void CopyTask::Submit(std::tuple<Resource*, Resource*, const void*>* Upload_Default_Data)
{
	m_UploadDefaultData.push_back(*Upload_Default_Data);
}

void CopyTask::copyResource(
	ID3D12GraphicsCommandList5* commandList,
	Resource* uploadResource, Resource* defaultResource,
	const void* data)
{
	// Set the data into the upload heap
	uploadResource->SetData(data);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultResource->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST));

	// To Defaultheap from Uploadheap
	commandList->CopyResource(
		defaultResource->GetID3D12Resource1(),	// Receiever
		uploadResource->GetID3D12Resource1());	// Sender

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultResource->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));
}