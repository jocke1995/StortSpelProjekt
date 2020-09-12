#include "stdafx.h"
#include "ForwardRenderTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../ConstantBufferView.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../Resource.h"
#include "../PipelineState.h"
#include "../Material.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../BaseCamera.h"
#include "../SwapChain.h"
#include "../RenderTarget.h"

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
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	const RenderTarget* swapChainRenderTarget = m_pSwapChain->GetRenderTarget(m_BackBufferIndex);
	ID3D12Resource1* swapChainResource = swapChainRenderTarget->GetResource()->GetID3D12Resource1();

	unsigned int a = 15;
	ID3D12Resource1* BrightTargetResource = m_RenderTargets["brightTarget"]->GetResource()->GetID3D12Resource1();

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	commandList->SetGraphicsRootSignature(m_pRootSig);
	
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtCBV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));
	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	DescriptorHeap* renderTargetHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];
	DescriptorHeap* depthBufferHeap  = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	// RenderTargets
	const unsigned int SwapChainIndex = swapChainRenderTarget->GetDescriptorHeapIndex();
	const unsigned int brightTargetIndex = m_RenderTargets["brightTarget"]->GetDescriptorHeapIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE cdhSwapChain = renderTargetHeap->GetCPUHeapAt(SwapChainIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE cdhBrightTarget = renderTargetHeap->GetCPUHeapAt(brightTargetIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE cdhs[] = { cdhSwapChain, cdhBrightTarget };

	// Depth
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(0);

	commandList->OMSetRenderTargets(2, cdhs, false, &dsh);

	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	commandList->ClearRenderTargetView(cdhSwapChain, clearColor, 0, nullptr);
	commandList->ClearRenderTargetView(cdhBrightTarget, clearColor, 0, nullptr);

	commandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	const D3D12_VIEWPORT viewPortSwapChain = *swapChainRenderTarget->GetRenderView()->GetViewPort();
	const D3D12_VIEWPORT viewPortBrightTarget = *m_RenderTargets["brightTarget"]->GetRenderView()->GetViewPort();
	const D3D12_VIEWPORT viewPorts[2] = { viewPortSwapChain, viewPortBrightTarget };

	const D3D12_RECT rectSwapChain = *swapChainRenderTarget->GetRenderView()->GetScissorRect();
	const D3D12_RECT rectBrightTarget = *m_RenderTargets["brightTarget"]->GetRenderView()->GetScissorRect();
	const D3D12_RECT rects[2] = { rectSwapChain, rectBrightTarget };

	const D3D12_RECT* rect = swapChainRenderTarget->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(2, viewPorts);
	commandList->RSSetScissorRects(2, rects);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set cbvs
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, m_Resources["cbPerFrame"]->GetGPUVirtualAdress());
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_SCENE, m_Resources["cbPerScene"]->GetGPUVirtualAdress());

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();

	// This pair for m_RenderComponents will be used for model-outlining in case any model is picked.
	std::pair<component::MeshComponent*, component::TransformComponent*> outlinedModel = std::make_pair(nullptr, nullptr);

	// Draw for every Rendercomponent with stencil testing disabled
	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
	for (int i = 0; i < m_RenderComponents.size(); i++)
	{
		component::MeshComponent* mc = m_RenderComponents.at(i).first;
		component::TransformComponent* tc = m_RenderComponents.at(i).second;

		// If the model is picked, we dont draw it with default stencil buffer.
		// Instead we store it and draw it later with a different pso to allow for model-outlining
		if (mc->IsPickedThisFrame() == true)
		{
			outlinedModel = std::make_pair(m_RenderComponents.at(i).first, m_RenderComponents.at(i).second);
			continue;
		}
		commandList->OMSetStencilRef(1);
		drawRenderComponent(mc, tc, viewProjMatTrans, commandList);
	}

	// Draw Rendercomponent with stencil testing enabled
	if (outlinedModel.first != nullptr)
	{
		commandList->SetPipelineState(m_PipelineStates[1]->GetPSO());
		commandList->OMSetStencilRef(1);
		drawRenderComponent(outlinedModel.first, outlinedModel.second, viewProjMatTrans, commandList);
	}
	
	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void FowardRenderTask::drawRenderComponent(
	component::MeshComponent* mc,
	component::TransformComponent* tc,
	const DirectX::XMMATRIX* viewProjTransposed,
	ID3D12GraphicsCommandList5* cl)
{
	// Check if the object is to be drawn in forwardRendering
	if (mc->GetDrawFlag() & FLAG_DRAW::ForwardRendering)
	{
		// Draw for every m_pMesh the meshComponent has
		for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
		{
			Mesh* m = mc->GetMesh(i);
			size_t num_Indices = m->GetNumIndices();
			const SlotInfo* info = m->GetSlotInfo();

			Transform* transform = tc->GetTransform();
			DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
			DirectX::XMMATRIX WVPTransposed = (*viewProjTransposed) * (*WTransposed);

			// Create a CB_PER_OBJECT struct
			CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

			cl->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);
			cl->SetGraphicsRootConstantBufferView(RS::CB_PER_OBJECT_CBV, m->GetMaterial()->GetConstantBufferView()->GetCBVResource()->GetGPUVirtualAdress());

			cl->IASetIndexBuffer(m->GetIndexBufferView());
			cl->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
		}
	}
}
