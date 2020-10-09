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
	const RenderTargetView* destinationRTV,
	unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	m_pSourceSRV = const_cast<ShaderResourceView*>(sourceSRV);
	m_pDestinationRTV = const_cast<RenderTargetView*>(destinationRTV);
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

	// The descriptorHeapIndices for the source&dest are currently put inside the textureSlots inside SlotInfo
	m_Info.textureAlbedo = m_pSourceSRV->GetDescriptorHeapIndex();
}

void DownSampleRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, NULL);

	commandList->SetGraphicsRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_RTV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::RTV];
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	// Change state on front/backbuffer
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	m_pDestinationRTV->GetResource()->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET));

	const D3D12_VIEWPORT* viewPort = m_pDestinationRTV->GetRenderView()->GetViewPort();
	const D3D12_RECT* rect = m_pDestinationRTV->GetRenderView()->GetScissorRect();
	commandList->RSSetViewports(1, viewPort);
	commandList->RSSetScissorRects(1, rect);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_RTV->GetCPUHeapAt(m_pDestinationRTV->GetDescriptorHeapIndex());
	commandList->OMSetRenderTargets(1, &cdh, false, nullptr);

	commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

	DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

	// Create a CB_PER_OBJECT struct
	CB_PER_OBJECT_STRUCT perObject = { identityMatrix, identityMatrix, m_Info };

	commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

	commandList->IASetIndexBuffer(m_pFullScreenQuadMesh->GetIndexBufferView());

	// Draw a fullscreen quad 
	commandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);

	// Change state on front/backbuffer
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
	//	m_pDestinationRTV->GetResource()->GetID3D12Resource1(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

	commandList->Close();
}
