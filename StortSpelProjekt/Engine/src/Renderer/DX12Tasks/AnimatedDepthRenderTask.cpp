#include "stdafx.h"
#include "AnimatedDepthRenderTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../PipelineState.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../Camera/BaseCamera.h"
#include "../SwapChain.h"

#include "../GPUMemory/Resource.h"
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/DepthStencil.h"
#include "../GPUMemory/DepthStencilView.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../GPUMemory/UnorderedAccessView.h"
#include "../GPUMemory/ConstantBuffer.h"

#include "../AnimatedMesh.h"
#include "../AnimatedModel.h"

AnimatedDepthRenderTask::AnimatedDepthRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	const std::wstring& psoName,
	unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	
}

AnimatedDepthRenderTask::~AnimatedDepthRenderTask()
{
}

void AnimatedDepthRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	commandList->SetGraphicsRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV0, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));
	commandList->SetGraphicsRootDescriptorTable(RS::dtUAV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	DescriptorHeap* depthBufferHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: Get Depth viewport, rightnow use swapchain since the view and rect is the same.
	const D3D12_VIEWPORT* viewPort = m_pSwapChain->GetRTV(0)->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = m_pSwapChain->GetRTV(0)->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();

	unsigned int index = m_pDepthStencil->GetDSV()->GetDescriptorHeapIndex();

	// Clear and set depth + stencil
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(index);
	commandList->OMSetRenderTargets(0, nullptr, false, &dsh);

	// Draw for every Rendercomponent
	for (int i = 0; i < m_RenderComponents.size(); i++)
	{
		component::ModelComponent* mc = m_RenderComponents.at(i).first;
		component::TransformComponent* tc = m_RenderComponents.at(i).second;

		// Draws all entities with ModelComponent + TransformComponent
		drawRenderComponent(mc, tc, viewProjMatTrans, commandList);
	}

	commandList->Close();
}

void AnimatedDepthRenderTask::drawRenderComponent(component::ModelComponent* mc, component::TransformComponent* tc, const DirectX::XMMATRIX* viewProjTransposed, ID3D12GraphicsCommandList5* cl)
{
	AnimatedModel* am = static_cast<AnimatedModel*>(mc->GetModel());
	// Draw for every m_pMesh the meshComponent has
	for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
	{
		AnimatedMesh* m = static_cast<AnimatedMesh*>(mc->GetMeshAt(i));
		size_t num_Indices = m->GetNumIndices();
		SlotInfo* info = const_cast<SlotInfo*>(mc->GetSlotInfoAt(i));

		// Assign srvs and uavs
		info->textureAlbedo = m->GetOrigVerticesSRV()->GetDescriptorHeapIndex();	// SRV1 orig vertices
		info->textureRoughness = m->GetVertexWeightSRV()->GetDescriptorHeapIndex();	// SRV2 vertexWeights
		info->textureMetallic = m->GetUAV()->GetDescriptorHeapIndex();				// UAV1 animatedVertices (to be animated in this pass)

		Transform* transform = tc->GetTransform();
		DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
		DirectX::XMMATRIX WVPTransposed = (*viewProjTransposed) * (*WTransposed);

		// CB_PER_OBJECT 
		CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

		unsigned int size = sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT);
		cl->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, size, &perObject, 0);

		cl->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, am->GetConstantBuffer()->GetDefaultResource()->GetID3D12Resource1()->GetGPUVirtualAddress());

		cl->IASetIndexBuffer(m->GetIndexBufferView());
		cl->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
	}
}
