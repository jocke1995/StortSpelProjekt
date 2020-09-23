#include "stdafx.h"
#include "SkyboxRenderTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../GPUMemory/ConstantBuffer.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../GPUMemory/Resource.h"
#include "../PipelineState.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../BaseCamera.h"
#include "../SwapChain.h"
#include "../GPUMemory/RenderTarget.h"
#include "../GPUMemory/RenderTargetView.h"

#include "../GPUMemory/ShaderResourceView.h"
#include "../Texture/TextureCubeMap.h"
#include "../ECS/Components/SkyboxComponent.h"

SkyboxRenderTask::SkyboxRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{

}

SkyboxRenderTask::~SkyboxRenderTask()
{
}

void SkyboxRenderTask::SetSkybox(component::SkyboxComponent* skybox)
{
	m_Skybox = skybox;
}

void SkyboxRenderTask::Execute() 
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

	// RenderTargets
	const unsigned int swapChainIndex = swapChainRenderTarget->GetDescriptorHeapIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE cdhSwapChain = renderTargetHeap->GetCPUHeapAt(swapChainIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE cdhs[] = { cdhSwapChain };

	// Depth
	D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(0);

	commandList->OMSetRenderTargets(1, cdhs, false, &dsh);

	const D3D12_VIEWPORT viewPortSwapChain = *swapChainRenderTarget->GetRenderView()->GetViewPort();
	const D3D12_VIEWPORT viewPorts[1] = { viewPortSwapChain };

	const D3D12_RECT rectSwapChain = *swapChainRenderTarget->GetRenderView()->GetScissorRect();
	const D3D12_RECT rects[1] = { rectSwapChain };

	const D3D12_RECT* rect = swapChainRenderTarget->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPorts);
	commandList->RSSetScissorRects(1, rects);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set cbvs
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, m_Resources["cbPerFrame"]->GetGPUVirtualAdress());
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_SCENE, m_Resources["cbPerScene"]->GetGPUVirtualAdress());

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	if (m_Skybox == nullptr)
	{
		return;
	}


	Mesh* m = m_Skybox->GetMesh();
	size_t num_Indices = m->GetNumIndices();

	SlotInfo info = { };
	info.vertexDataIndex = m_Skybox->GetMesh()->m_pSRV->GetDescriptorHeapIndex();
	info.textureAmbient = m_Skybox->GetTexture()->GetDescriptorHeapIndex();

	Transform* transform = m_Skybox->GetTransform();
	DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
	DirectX::XMMATRIX WVPTransposed = (*viewProjMatTrans) * (*WTransposed);

	// Create a CB_PER_OBJECT struct
	CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, info };

	commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

	commandList->IASetIndexBuffer(m->GetIndexBufferView());
	commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
	

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
