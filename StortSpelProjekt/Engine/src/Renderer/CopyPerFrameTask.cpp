#include "CopyPerFrameTask.h"

CopyPerFrameTask::CopyPerFrameTask(ID3D12Device5* device)
	:CopyTask(device)
{

}

CopyPerFrameTask::~CopyPerFrameTask()
{

}

void CopyPerFrameTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);

	this->commandInterface->Reset(this->commandInterfaceIndex);

	for (auto& pair : this->data_CBVs)
	{
		this->CopyResource(
			commandList,
			pair.second->GetUploadResource(),
			pair.second->GetCBVResource(),
			pair.first);	// Data
	}

	commandList->Close();
}

void CopyPerFrameTask::CopyResource(
	ID3D12GraphicsCommandList5* commandList,
	Resource* uploadResource, Resource* defaultResource,
	void* data)
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
