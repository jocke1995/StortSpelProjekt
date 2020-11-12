#include "stdafx.h"
#include "ParticleRenderTask.h"

#include "../RenderView.h"
#include "../RootSignature.h"
#include "../CommandInterface.h"
#include "../GPUMemory/RenderTargetView.h"
#include "../GPUMemory/DepthStencil.h"
#include "../GPUMemory/DepthStencilView.h"
#include "../GPUMemory/ConstantBufferView.h"
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
#include "../Renderer/GPUMemory/Resource.h"

ParticleRenderTask::ParticleRenderTask(ID3D12Device5* device, RootSignature* rootSignature, 
	const std::wstring& VSName, const std::wstring& PSName, 
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds, 
	const std::wstring& psoName, unsigned int FLAG_THREAD)
	:RenderTask(device, rootSignature, VSName, PSName, gpsds, psoName, FLAG_THREAD)
{
	AssetLoader* al = AssetLoader::Get();
	m_pParticleMesh = al->LoadModel(L"../Vendor/Resources/Models/Quad/NormalizedQuad.obj")->GetMeshAt(0);
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
			ParticleEffect* effect = &pec->m_ParticleEffect;

			// Test to multiply in render (Later do it in compute)
			commandList->SetGraphicsRootShaderResourceView(RS::SRV0, effect->m_pSRV->GetResource()->GetGPUVirtualAdress());

			Texture2DGUI* texture = pec->GetParticleEffect()->GetTexture();

			// Draw a quad (m_pParticleQuad)
			size_t num_Indices = m_pParticleMesh->GetNumIndices();
			SlotInfo info = {};
			info.vertexDataIndex = m_pParticleMesh->GetSRV()->GetDescriptorHeapIndex();
			info.textureAlbedo = texture->GetDescriptorHeapIndex();

			float3 pos = tc->GetTransform()->GetPositionFloat3();
			DirectX::XMMATRIX tcPos = {
			pos.x, pos.y, pos.z, 1,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
			};
			DirectX::XMMATRIX VPTransposed = *viewProjMatTrans;

			// Create a CB_PER_OBJECT struct
			// Hack: sending in tcPos specially in this renderTask
			CB_PER_OBJECT_STRUCT perObject = { tcPos, VPTransposed, info };

			commandList->SetGraphicsRoot32BitConstants(RS::CB_PER_OBJECT_CONSTANTS, sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT), &perObject, 0);

			commandList->IASetIndexBuffer(m_pParticleMesh->GetIndexBufferView());
			commandList->DrawIndexedInstanced(num_Indices, effect->m_Settings.particleCount, 0, 0, 0);
		}
	}

	// Change state on front/backbuffer
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	commandList->Close();
}
