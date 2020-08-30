#include "BlendRenderTask.h"

BlendRenderTask::BlendRenderTask(	
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
	
}

BlendRenderTask::~BlendRenderTask()
{
}

void BlendRenderTask::Execute()
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

	SwapChain* sc = static_cast<SwapChain*>(this->renderTargets["swapChain"]);
	const D3D12_VIEWPORT* viewPort = sc->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = sc->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a CB_PER_FRAME struct
	CB_PER_FRAME_STRUCT perFrame = { camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z };
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, this->resources["cbPerFrame"]->GetGPUVirtualAdress());
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_SCENE, this->resources["cbPerScene"]->GetGPUVirtualAdress());

	const XMMATRIX * viewProjMatTrans = this->camera->GetViewProjectionTranposed();

	// Draw from opposite order from the sorted array
	for(int i = this->renderComponents.size() - 1; i >= 0; i--)
	{
		component::MeshComponent* mc = this->renderComponents.at(i).first;
		component::TransformComponent* tc = this->renderComponents.at(i).second;

		// Check if the renderComponent is to be drawn in Blend
		if (mc->GetDrawFlag() & FLAG_DRAW::Blend)
		{
			// Draw for every mesh the MeshComponent has
			for (unsigned int j = 0; j < mc->GetNrOfMeshes(); j++)
			{
				Mesh* m = mc->GetMesh(j);
				size_t num_Indices = m->GetNumIndices();
				const SlotInfo* info = m->GetSlotInfo();

				Transform* transform = tc->GetTransform();

				XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
				XMMATRIX WVPTransposed = (*viewProjMatTrans) * (*WTransposed);

				// Create a CB_PER_OBJECT struct
				CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed , *info };

				commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);
				commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_OBJECT_CBV, m->GetMaterial()->GetConstantBufferView()->GetCBVResource()->GetGPUVirtualAdress());

				commandList->IASetIndexBuffer(mc->GetMesh(j)->GetIndexBufferView());
				// Draw each object twice with different PSO 
				for (int k = 0; k < 2; k++)
				{
					commandList->SetPipelineState(this->pipelineStates[k]->GetPSO());
					commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
				}
			}
		}	
	}
	
	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
