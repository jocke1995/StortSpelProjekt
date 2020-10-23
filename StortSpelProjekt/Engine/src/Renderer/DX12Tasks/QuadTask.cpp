#include "stdafx.h"
#include "QuadTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../PipelineState.h"
#include "../QuadManager.h"
#include "../Mesh.h"

#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/Resource.h"

#include "../../ECS/Components/GUI2DComponent.h"
#include "../../Renderer/QuadManager.h"

QuadTask::QuadTask(ID3D12Device5* device,
	RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	const std::wstring& psoName,
	unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
}

QuadTask::~QuadTask()
{
}

void QuadTask::SetQuadComponents(std::vector<component::GUI2DComponent*>* quadComponents)
{
	m_QuadComponents = *quadComponents;
}

void QuadTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	const RenderTargetView* swapChainRenderTarget = m_pSwapChain->GetRTV(m_BackBufferIndex);
	ID3D12Resource1* swapChainResource = swapChainRenderTarget->GetResource()->GetID3D12Resource1();

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	commandList->SetGraphicsRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	DescriptorHeap* renderTargetHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();

	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	// set the text pipeline state object
	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	// this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// bind the text srv
	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	unsigned int renderTargetIndex = m_pSwapChain->GetRTV(m_BackBufferIndex)->GetDescriptorHeapIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE cdhSwapChain = renderTargetHeap->GetCPUHeapAt(renderTargetIndex);
	commandList->OMSetRenderTargets(1, &cdhSwapChain, true, nullptr);

	commandList->RSSetViewports(1, swapChainRenderTarget->GetRenderView()->GetViewPort());
	commandList->RSSetScissorRects(1, swapChainRenderTarget->GetRenderView()->GetScissorRect());

	draw(commandList);

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void QuadTask::draw(ID3D12GraphicsCommandList5* commandList)
{
	for (int i = 0; i < m_QuadComponents.size(); i++)
	{
		QuadManager* qm = m_QuadComponents.at(i)->GetQuadManager();

		// Create a CB_PER_GUI2D_OBJECT_STRUCT struct
		size_t num_Indices = qm->GetQuad()->GetNumIndices();
		const SlotInfo* info = qm->GetSlotInfo();

		float4 textureInfo = float4{ 0.0 };
		textureInfo.x = qm->HasTexture();
		textureInfo.y = qm->GetActiveTexture();
		CB_PER_GUI2D_OBJECT_STRUCT perObject = { qm->GetAmountOfBlend(), textureInfo, *info };

		commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_GUI2D_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);
		commandList->IASetIndexBuffer(qm->GetQuad()->GetIndexBufferView());
		commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
	}
}
