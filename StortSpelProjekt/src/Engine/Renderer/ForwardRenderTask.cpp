#include "ForwardRenderTask.h"

// Forceinclude issue with intellisense
#include "../Headers/stdafx.h"

FowardRenderTask::FowardRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
	
}

FowardRenderTask::~FowardRenderTask()
{
}

void FowardRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = this->commandInterface->GetCommandAllocator(this->commandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = this->commandInterface->GetCommandList(this->commandInterfaceIndex);
	ID3D12Resource1* swapChainResource = this->renderTargets["swapChain"]->GetResource(this->backBufferIndex)->GetID3D12Resource1();

	this->commandInterface->Reset(this->commandInterfaceIndex);

	commandList->SetGraphicsRootSignature(this->rootSig);
	
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtCBV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));
	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	DescriptorHeap* renderTargetHeap = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];
	DescriptorHeap* depthBufferHeap  = this->descriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetHeap->GetCPUHeapAt(this->backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(0);

	commandList->OMSetRenderTargets(1, &cdh, true, &dsh);

	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	commandList->ClearRenderTargetView(cdh, clearColor, 0, nullptr);

	commandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	SwapChain* sc = static_cast<SwapChain*>(this->renderTargets["swapChain"]);
	const D3D12_VIEWPORT* viewPort = sc->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = sc->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set cbvs
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, this->resources["cbPerFrame"]->GetGPUVirtualAdress());
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_SCENE, this->resources["cbPerScene"]->GetGPUVirtualAdress());

	const XMMATRIX* viewProjMatTrans = this->camera->GetViewProjectionTranposed();

	// This pair for renderComponents will be used for model-outlining in case any model is picked.
	std::pair<component::MeshComponent*, component::TransformComponent*> outlinedModel = std::make_pair(nullptr, nullptr);

	// Draw for every Rendercomponent with stencil testing disabled
	commandList->SetPipelineState(this->pipelineStates[0]->GetPSO());
	for (int i = 0; i < this->renderComponents.size(); i++)
	{
		component::MeshComponent* mc = this->renderComponents.at(i).first;
		component::TransformComponent* tc = this->renderComponents.at(i).second;

		// If the model is picked, we dont draw it with default stencil buffer.
		// Instead we store it and draw it later with a different pso to allow for model-outlining
		if (mc->IsPickedThisFrame() == true)
		{
			//Log::Print("%s is picked!\n", mc->GetParent()->GetName().c_str());
			outlinedModel = std::make_pair(this->renderComponents.at(i).first, this->renderComponents.at(i).second);
			continue;
		}

		this->DrawRenderComponent(mc, tc, viewProjMatTrans, commandList);
	}

	// Draw Rendercomponent with stencil testing enabled
	if (outlinedModel.first != nullptr)
	{
		//static unsigned int counter = 0;
		//counter++;
		//Entity* parent = outlinedModel.first->GetParent();
		//Log::Print("%s is picked! %d\n", parent->GetName().c_str(), counter);

		commandList->SetPipelineState(this->pipelineStates[1]->GetPSO());
		commandList->OMSetStencilRef(1);
		this->DrawRenderComponent(outlinedModel.first, outlinedModel.second, viewProjMatTrans, commandList);
	}
	

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void FowardRenderTask::DrawRenderComponent(
	component::MeshComponent* mc,
	component::TransformComponent* tc,
	const DirectX::XMMATRIX* viewProjTransposed,
	ID3D12GraphicsCommandList5* cl)
{
	// Check if the object is to be drawn in forwardRendering
	if (mc->GetDrawFlag() & FLAG_DRAW::ForwardRendering)
	{
		// Draw for every mesh the meshComponent has
		for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
		{
			size_t num_Indices = mc->GetMesh(i)->GetNumIndices();
			const SlotInfo* info = mc->GetMesh(i)->GetSlotInfo();

			Transform* transform = tc->GetTransform();
			XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
			XMMATRIX WVPTransposed = (*viewProjTransposed) * (*WTransposed);

			// Create a CB_PER_OBJECT struct
			CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

			cl->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

			cl->IASetIndexBuffer(mc->GetMesh(i)->GetIndexBufferView());
			cl->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
		}
	}
}
