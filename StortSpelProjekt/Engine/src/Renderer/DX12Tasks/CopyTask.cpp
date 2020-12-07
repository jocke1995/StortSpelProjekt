#include "stdafx.h"
#include "CopyTask.h"

#include "../GPUMemory/Resource.h"
#include "../CommandInterface.h"

CopyTask::CopyTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD)
	:DX12Task(device, interfaceType, FLAG_THREAD)
{

}

CopyTask::~CopyTask()
{

}

void CopyTask::Submit(std::tuple<Resource*, Resource*>* Upload_Default)
{
	m_UploadDefault.push_back(*Upload_Default);
}

void CopyTask::copyResource(
	ID3D12GraphicsCommandList5* commandList,
	Resource* uploadResource, Resource* defaultResource)
{
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