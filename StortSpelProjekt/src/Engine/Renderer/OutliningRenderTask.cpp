#include "OutliningRenderTask.h"

OutliningRenderTask::OutliningRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
	// Init with nullptr
	this->Clear();
}

OutliningRenderTask::~OutliningRenderTask()
{
}

void OutliningRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);
	ID3D12Resource1* swapChainResource = this->renderTargets["swapChain"]->GetResource(this->backBufferIndex)->GetID3D12Resource1();

	this->commandInterface->Reset(this->commandInterfaceIndex);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	DescriptorHeap* renderTargetHeap = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];
	DescriptorHeap* depthBufferHeap = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetHeap->GetCPUHeapAt(this->backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(0);

	// Check if there is an object to outline
	if (this->objectToOutline.first == nullptr)
	{
		commandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, 0, nullptr);
		commandList->Close();
		return;
	}
	// else continue as usual

	commandList->SetGraphicsRootSignature(this->rootSig);
	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	commandList->OMSetRenderTargets(1, &cdh, true, &dsh);

	SwapChain* sc = static_cast<SwapChain*>(this->renderTargets["swapChain"]);
	const D3D12_VIEWPORT* viewPort = sc->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = sc->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetPipelineState(this->pipelineStates[0]->GetPSO());

	const XMMATRIX* viewProjMatTrans = this->camera->GetViewProjectionTranposed();

	// Draw for every mesh
	for (int i = 0; i < this->objectToOutline.first->GetNrOfMeshes(); i++)
	{
		Mesh* m = this->objectToOutline.first->GetMesh(i);
		Transform* t = this->objectToOutline.second->GetTransform();
		Transform newScaledTransform = *t;
		newScaledTransform.IncreaseScaleByPercent(0.02f);
		newScaledTransform.UpdateWorldMatrix();

		size_t num_Indices = m->GetNumIndices();
		const SlotInfo* info = m->GetSlotInfo();

		XMMATRIX* WTransposed = newScaledTransform.GetWorldMatrixTransposed();
		XMMATRIX WVPTransposed = (*viewProjMatTrans) * (*WTransposed);

		// Create a CB_PER_OBJECT struct
		CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

		commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

		commandList->IASetIndexBuffer(m->GetIndexBufferView());

		commandList->OMSetStencilRef(1);
		commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
	}

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void OutliningRenderTask::SetObjectToOutline(std::pair<component::MeshComponent*, component::TransformComponent*>* objectToOutline)
{
	this->objectToOutline = *objectToOutline;
}

void OutliningRenderTask::Clear()
{
	this->objectToOutline.first = nullptr;
	this->objectToOutline.second = nullptr;
}
