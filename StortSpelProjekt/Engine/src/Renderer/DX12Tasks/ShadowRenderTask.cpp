#include "stdafx.h"
#include "ShadowRenderTask.h"

#include "../DescriptorHeap.h"
#include "../GPUMemory/Resource.h"
#include "../RenderView.h"
#include "../GPUMemory/DepthStencilView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../PipelineState.h"
#include "../ShadowInfo.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/BaseCamera.h"

ShadowRenderTask::ShadowRenderTask(
	ID3D12Device5* device,
	RootSignature* rootSignature,
	LPCWSTR VSName, LPCWSTR PSName,
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
	LPCTSTR psoName)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName)
{

}

ShadowRenderTask::~ShadowRenderTask()
{
}

void ShadowRenderTask::AddShadowCastingLight(std::pair<Light*, ShadowInfo*> light)
{
	m_lights.push_back(light);
}

void ShadowRenderTask::ClearSpecificLight(Light* light)
{
	unsigned int i = 0;
	for (auto& pair : m_lights)
	{
		if (pair.first == light)
		{
			m_lights.erase(m_lights.begin() + i);
		}
		i++;
	}
}

void ShadowRenderTask::Clear()
{
	m_lights.clear();
}

void ShadowRenderTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	m_pCommandInterface->Reset(m_CommandInterfaceIndex);
	
	commandList->SetGraphicsRootSignature(m_pRootSig);

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV];
	ID3D12DescriptorHeap* d3d12DescriptorHeap = descriptorHeap_CBV_UAV_SRV->GetID3D12DescriptorHeap();
	commandList->SetDescriptorHeaps(1, &d3d12DescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

	DescriptorHeap* depthBufferHeap = m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::DSV];

	// Draw for every shadow-casting-light
	for (auto pair : m_lights)
	{
		commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const D3D12_VIEWPORT* viewPort = pair.second->GetRenderView()->GetViewPort();
		const D3D12_RECT* rect = pair.second->GetRenderView()->GetScissorRect();
		commandList->RSSetViewports(1, viewPort);
		commandList->RSSetScissorRects(1, rect);

		const DirectX::XMMATRIX* viewProjMatTrans = pair.first->GetCamera()->GetViewProjectionTranposed();

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			pair.second->GetResource()->GetID3D12Resource1(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Clear and set depthstencil
		unsigned int dsvIndex = pair.second->GetDSV()->GetDescriptorHeapIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE dsh = depthBufferHeap->GetCPUHeapAt(dsvIndex);
		commandList->ClearDepthStencilView(dsh, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		commandList->OMSetRenderTargets(0, nullptr, true, &dsh);

		// Draw for every Rendercomponent
		for (int i = 0; i < m_RenderComponents.size(); i++)
		{
			component::ModelComponent* mc = m_RenderComponents.at(i).first;
			component::TransformComponent* tc = m_RenderComponents.at(i).second;

			// Check if the object is to be drawn in ShadowPass
			if (mc->GetDrawFlag() & FLAG_DRAW::Shadow)
			{
				// Draw for every m_pMesh the meshComponent has
				for (unsigned int i = 0; i < mc->GetNrOfMeshes(); i++)
				{
					size_t num_Indices = mc->GetMeshAt(i)->GetNumIndices();
					const SlotInfo* info = mc->GetSlotInfoAt(i);

					Transform* transform = tc->GetTransform();
					DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
					DirectX::XMMATRIX WVPTransposed = (*viewProjMatTrans) * (*WTransposed);

					// Create a CB_PER_OBJECT struct
					CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed, *info };

					commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

					commandList->IASetIndexBuffer(mc->GetMeshAt(i)->GetIndexBufferView());
					commandList->DrawIndexedInstanced(num_Indices, 1, 0, 0, 0);
				}
			}
		}

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			pair.second->GetResource()->GetID3D12Resource1(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
	commandList->Close();
}
