#include "stdafx.h"
#include "DownSampleRenderTask.h"

#include "../CommandInterface.h"

#include "../SwapChain.h"
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../GPUMemory/UnorderedAccessView.h"
#include "../RenderView.h"
#include "../GPUMemory/Resource.h"

#include "../DescriptorHeap.h"
#include "../RootSignature.h"
#include "../PipelineState.h"

#include "../Mesh.h"

DownSampleRenderTask::DownSampleRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	const std::wstring& VSName, const std::wstring& PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	const std::wstring& psoName,
	const ShaderResourceView* sourceSRV,
	const UnorderedAccessView* destinationUAV,
	unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	m_pSourceSRV = const_cast<ShaderResourceView*>(sourceSRV);
	m_pDestinationUAV = const_cast<UnorderedAccessView*>(destinationUAV);
	m_NumIndices = 0;
	m_Info = {};
}

DownSampleRenderTask::~DownSampleRenderTask()
{
}

void DownSampleRenderTask::SetFullScreenQuad(Mesh* mesh)
{
	m_pFullScreenQuadMesh = mesh;
}

void DownSampleRenderTask::SetFullScreenQuadInSlotInfo()
{
	// Mesh
	m_NumIndices = m_pFullScreenQuadMesh->GetNumIndices();
	m_Info.vertexDataIndex = m_pFullScreenQuadMesh->m_pSRV->GetDescriptorHeapIndex();
}

void DownSampleRenderTask::Execute()
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

	commandList->OMSetRenderTargets(0, nullptr, true, nullptr);

	const D3D12_VIEWPORT* viewPort = swapChainRenderTarget->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = swapChainRenderTarget->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	// Draw a fullscreen quad 
	// The descriptorHeapIndices for the SRVs are currently put inside the textureSlots inside SlotInfo
	m_Info.textureMetallic = m_pSwapChain->GetSRV(m_BackBufferIndex)->GetDescriptorHeapIndex();

	DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

	// Create a CB_PER_OBJECT struct
	CB_PER_OBJECT_STRUCT perObject = { identityMatrix, identityMatrix, m_Info };

	commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

	commandList->IASetIndexBuffer(m_pFullScreenQuadMesh->GetIndexBufferView());

	commandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);


	commandList->Close();
}
