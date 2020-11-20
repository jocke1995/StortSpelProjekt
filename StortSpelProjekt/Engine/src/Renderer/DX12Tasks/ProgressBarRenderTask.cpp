#include "stdafx.h"
#include "ProgressBarRenderTask.h"

// Misc
#include "../RenderView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../PipelineState.h"
#include "../Renderer/Camera/BaseCamera.h"
#include "../Renderer/Mesh.h"

// GPU-Memory
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/DepthStencil.h"
#include "../GPUMemory/DepthStencilView.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../GPUMemory/ConstantBuffer.h"
#include "../GPUMemory/ConstantBufferView.h"
#include "../GPUMemory/Resource.h"

// Progressbar specifics
#include "../ECS/Components/ProgressBarComponent.h"

ProgressBarRenderTask::ProgressBarRenderTask(ID3D12Device5* device, RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName, 
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
	const std::wstring& psoName, unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	
}

ProgressBarRenderTask::~ProgressBarRenderTask()
{
}

void ProgressBarRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	const RenderTargetView* swapChainRenderTarget = m_pSwapChain->GetRTV(m_BackBufferIndex);
	ID3D12Resource1* swapChainResource = swapChainRenderTarget->GetResource()->GetID3D12Resource1();

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
	DescriptorHeap* depthBufferHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	unsigned int renderTargetIndex = m_pSwapChain->GetRTV(m_BackBufferIndex)->GetDescriptorHeapIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetHeap->GetCPUHeapAt(renderTargetIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(m_pDepthStencil->GetDSV()->GetDescriptorHeapIndex());

	commandList->OMSetRenderTargets(1, &cdh, true, &dsh);

	const D3D12_VIEWPORT* viewPort = swapChainRenderTarget->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = swapChainRenderTarget->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, m_Resources["cbPerFrame"]->GetGPUVirtualAdress());

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();
	DirectX::XMMATRIX VPTransposed = *viewProjMatTrans;

	SlotInfo info = {};
	info.vertexDataIndex = m_pQuad->GetSRV()->GetDescriptorHeapIndex();

	// Draw a quad (m_pParticleQuad)
	size_t num_Indices = m_pQuad->GetNumIndices();

	for (component::ProgressBarComponent* pbc: m_ProgressBarComponents)
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			// Set the constantBuffer
			D3D12_GPU_VIRTUAL_ADDRESS gpuVA = pbc->m_ConstantBuffers[i]->GetCBV()->GetResource()->GetGPUVirtualAdress();
			commandList->SetGraphicsRootConstantBufferView(RS::CBV0, gpuVA);
			
			//info.textureAlbedo = texture->GetDescriptorHeapIndex();

			// Create a CB_PER_OBJECT struct
			// Hack: sending in tcPos specially in this renderTask
			CB_PER_OBJECT_STRUCT perObject = { {}, VPTransposed, info };

			commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

			commandList->IASetIndexBuffer(m_pQuad->GetIndexBufferView());
			commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
		}
	}

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void ProgressBarRenderTask::SetBillboardMesh(Mesh* quadMesh)
{
	m_pQuad = quadMesh;
}

void ProgressBarRenderTask::SetProgressBarComponents(std::vector<component::ProgressBarComponent*>* progressBarComponents)
{
	m_ProgressBarComponents = *progressBarComponents;
}
