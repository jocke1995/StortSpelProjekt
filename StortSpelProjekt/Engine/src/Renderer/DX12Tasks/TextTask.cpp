#include "stdafx.h"
#include "TextTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../ConstantBufferView.h"
#include "../CommandInterface.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../RenderTarget.h"
#include "../Resource.h"
#include "../PipelineState.h"
#include "../Text.h"
#include "../../ECS/Components/TextComponent.h"

TextTask::TextTask(ID3D12Device5* device, 
	RootSignature* rootSignature, 
	LPCWSTR VSName, LPCWSTR PSName, 
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
	LPCTSTR psoName) :RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{
}

TextTask::~TextTask()
{
}

void TextTask::SetTextComponents(std::vector<component::TextComponent*>* textComponents)
{
	m_TextComponents = *textComponents;
}

void TextTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	const RenderTarget* swapChainRenderTarget = m_pSwapChain->GetRenderTarget(m_BackBufferIndex);
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
  
	D3D12_CPU_DESCRIPTOR_HANDLE cdhSwapChain = renderTargetHeap->GetCPUHeapAt(m_BackBufferIndex);
	commandList->OMSetRenderTargets(1, &cdhSwapChain, true, nullptr);

	commandList->RSSetViewports(1, swapChainRenderTarget->GetRenderView()->GetViewPort());
	commandList->RSSetScissorRects(1, swapChainRenderTarget->GetRenderView()->GetScissorRect());

	for (int i = 0; i < m_TextComponents.size(); i++)
	{
		component::TextComponent* tc = m_TextComponents.at(i);
		draw(commandList, tc);
	}

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}

void TextTask::draw(ID3D12GraphicsCommandList5* commandList, component::TextComponent* tc)
{
	int nrOfCharacters;
	for (int i = 0; i < tc->GetNumOfTexts(); i++)
	{
		Text* text = tc->GetText(i);

		// Create a CB_PER_OBJECT struct
		SlotInfo* info = text->GetSlotInfo();
		DirectX::XMMATRIX idMatrix = DirectX::XMMatrixIdentity();
		CB_PER_OBJECT_STRUCT perObject = { idMatrix, idMatrix, *info };
		commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

		// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
		nrOfCharacters = text->GetNrOfCharacters();
		commandList->DrawInstanced(4, nrOfCharacters, 0, 0);
	}
}
