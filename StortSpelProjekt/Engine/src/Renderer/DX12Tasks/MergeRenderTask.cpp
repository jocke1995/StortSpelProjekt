#include "stdafx.h"
#include "MergeRenderTask.h"

#include "../CommandInterface.h"

#include "../SwapChain.h"
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../RenderView.h"
#include "../GPUMemory/Resource.h"

#include "../DescriptorHeap.h"
#include "../RootSignature.h"
#include "../PipelineState.h"

#include "../Mesh.h"

MergeRenderTask::MergeRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	const std::wstring& psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
	m_NumIndices = 0;
	m_Info = {};
}

MergeRenderTask::~MergeRenderTask()
{
}

void MergeRenderTask::AddSRVIndexToMerge(unsigned int srvIndex)
{
	m_SRVIndices.push_back(srvIndex);
}

void MergeRenderTask::SetFullScreenQuad(Mesh* mesh)
{
	m_pFullScreenQuadMesh = mesh;
}

void MergeRenderTask::CreateSlotInfo()
{
	// Mesh
	m_NumIndices = m_pFullScreenQuadMesh->GetNumIndices();
	m_Info.vertexDataIndex = m_pFullScreenQuadMesh->m_pSRV->GetDescriptorHeapIndex();

	// Textures
	// The descriptorHeapIndices for the SRVs are currently put inside the textureSlots inside SlotInfo
	m_Info.textureDiffuse = m_SRVIndices[0];	// Blurred srv
}

void MergeRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	// Get renderTarget
	const RenderTargetView* swapChainRenderTarget = m_pSwapChain->GetRTV(m_BackBufferIndex);
	ID3D12Resource1* swapChainResource = swapChainRenderTarget->GetResource()->GetID3D12Resource1();

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	DescriptorHeap* renderTargetHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];

	const unsigned int SwapChainIndex = swapChainRenderTarget->GetDescriptorHeapIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetHeap->GetCPUHeapAt(SwapChainIndex);

	commandList->SetGraphicsRootSignature(m_pRootSig);

	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	commandList->OMSetRenderTargets(1, &cdh, true, nullptr);

	const D3D12_VIEWPORT* viewPort = swapChainRenderTarget->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = swapChainRenderTarget->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	// Draw a fullscreen quad 
	// The descriptorHeapIndices for the SRVs are currently put inside the textureSlots inside SlotInfo
	m_Info.textureAmbient = m_pSwapChain->GetSRV(m_BackBufferIndex)->GetDescriptorHeapIndex();

	DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

	// Create a CB_PER_OBJECT struct
	CB_PER_OBJECT_STRUCT perObject = { identityMatrix, identityMatrix, m_Info };

	commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

	commandList->IASetIndexBuffer(m_pFullScreenQuadMesh->GetIndexBufferView());

	commandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
