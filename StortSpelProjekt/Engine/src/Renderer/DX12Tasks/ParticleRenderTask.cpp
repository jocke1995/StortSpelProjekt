#include "stdafx.h"
#include "ParticleRenderTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/DepthStencil.h"
#include "../GPUMemory/DepthStencilView.h"
#include "../GPUMemory/ShaderResourceView.h"
#include "../GPUMemory/UnorderedAccessView.h"
#include "../DescriptorHeap.h"
#include "../SwapChain.h"
#include "../GPUMemory/Resource.h"
#include "../PipelineState.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/Camera/BaseCamera.h"

// Includes used for particles to work
#include "../Misc/AssetLoader.h"
#include "../Renderer/Model.h"
#include "../ECS/Entity.h"
#include "../Renderer/Texture/Texture2DGUI.h"
#include "../ECS/Components/ParticleEmitterComponent.h"
#include "../Particles/ParticleEffect.h"

ParticleRenderTask::ParticleRenderTask(ID3D12Device5* device, RootSignature* rootSignature, const std::wstring& VSName, const std::wstring& PSName, std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, const std::wstring& psoName, unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	AssetLoader* al = AssetLoader::Get();
	
}

ParticleRenderTask::~ParticleRenderTask()
{
}

void ParticleRenderTask::Execute()
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
	commandList->SetGraphicsRootDescriptorTable(RS::dtSRV0, descriptorHeap_CBV_UAV_SRV->GetGPUHeapAt(0));

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

	// Create a CB_PER_FRAME struct
	CB_PER_FRAME_STRUCT perFrame = { m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z };
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_FRAME, m_Resources["cbPerFrame"]->GetGPUVirtualAdress());
	commandList->SetGraphicsRootConstantBufferView(RS::CB_PER_SCENE, m_Resources["cbPerScene"]->GetGPUVirtualAdress());

	const DirectX::XMMATRIX* viewProjMatTrans = m_pCamera->GetViewProjectionTranposed();

	// Draw from opposite order from the sorted array
	for (int i = m_RenderComponents.size() - 1; i >= 0; i--)
	{
		component::ModelComponent* mc = m_RenderComponents.at(i).first;
		component::TransformComponent* tc = m_RenderComponents.at(i).second;

		// It's a particle
		if (mc == nullptr)
		{
			component::ParticleEmitterComponent* pec = tc->GetParent()->GetComponent<component::ParticleEmitterComponent>();
			if (pec == nullptr)
			{
				Log::PrintSeverity(Log::Severity::CRITICAL, "ParticleComponent was nullptr!!, plez fix\n");
				return;
			}
			ParticleEffect* effect = pec->m_pParticleEffect;
			commandList->SetGraphicsRootShaderResourceView(RS::SRV0, effect->m_pSRV->GetResource()->GetGPUVirtualAdress());
			commandList->SetPipelineState(m_PipelineStates[0]->GetPSO());

			Texture2DGUI* texture = pec->GetTexture();

			// Draw a quad (m_pParticleQuad)
			size_t num_Indices = effect->m_pMesh->GetNumIndices();
			SlotInfo info = {};
			info.vertexDataIndex = effect->m_pMesh->GetSRV()->GetDescriptorHeapIndex();
			info.textureAlbedo = texture->GetDescriptorHeapIndex();

			// Get uav
			Transform* transform = tc->GetTransform();


			DirectX::XMMATRIX* WTransposed = transform->GetWorldMatrixTransposed();
			DirectX::XMMATRIX WVPTransposed = (*viewProjMatTrans) * (*WTransposed);

			// Create a CB_PER_OBJECT struct
			CB_PER_OBJECT_STRUCT perObject = { *WTransposed, WVPTransposed , info };

			commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

			commandList->IASetIndexBuffer(effect->m_pMesh->GetIndexBufferView());
			commandList->DrawIndexedInstanced(num_Indices, effect->m_ParticleCount, 0, 0, 0);
		}
	}

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
