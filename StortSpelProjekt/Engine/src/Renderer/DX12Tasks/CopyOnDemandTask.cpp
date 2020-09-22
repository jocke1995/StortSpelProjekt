#include "stdafx.h"
#include "CopyOnDemandTask.h"

#include "../Texture/Texture.h"
#include "../Resource.h"
#include "../CommandInterface.h"

#include "../Texture/TextureCubeMap.h"

CopyOnDemandTask::CopyOnDemandTask(ID3D12Device5* device)
	:CopyTask(device)
{

}

CopyOnDemandTask::~CopyOnDemandTask()
{
}

void CopyOnDemandTask::Clear()
{
	m_UploadDefaultData.clear();
	m_Textures.clear();
}

void CopyOnDemandTask::SubmitTexture(Texture* texture)
{
	m_Textures.push_back(texture);
}

void CopyOnDemandTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);
	
	m_pCommandInterface->Reset(m_CommandInterfaceIndex);
	
	// record the "small" data, such as constantbuffers..
	for (auto& tuple : m_UploadDefaultData)
	{
		copyResource(
			commandList,
			std::get<0>(tuple),		// UploadHeap
			std::get<1>(tuple),		// DefaultHeap
			std::get<2>(tuple));	// Data
	}

	// record texturedata
	for (Texture* texture : m_Textures)
	{
		copyTexture(commandList, texture);
	}
	
	commandList->Close();
}

void CopyOnDemandTask::copyTexture(ID3D12GraphicsCommandList5* commandList, Texture* texture)
{
	ID3D12Resource* defaultHeap = texture->m_pDefaultResource->GetID3D12Resource1();
	ID3D12Resource* uploadHeap = texture->m_pUploadResource->GetID3D12Resource1();

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultHeap,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST));
	
	// Check type
	if (texture->m_Type == TEXTURE_TYPE::TEXTURE2D)
	{
		// Transfer the data
		UpdateSubresources(commandList,
			defaultHeap, uploadHeap,
			0, 0, 1,
			&texture->m_SubresourceData);
	}
	else if (texture->m_Type == TEXTURE_TYPE::TEXTURECUBEMAP)
	{
		TextureCubeMap* cubemap = static_cast<TextureCubeMap*>(texture);
		unsigned int i = cubemap->GetDescriptorHeapIndex();
		// Transfer the data
		UpdateSubresources(commandList,
			defaultHeap, uploadHeap,
			0, 0, cubemap->subResourceData.size(), // Should always be 6
			cubemap->subResourceData.data());
	}
	else
	{
		// Not supporting this texture type currently
		Log::PrintSeverity(Log::Severity::CRITICAL, "CopyOnDemand: Not supporting TEXTURE_TYPE: %d", texture->m_Type);
	}

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultHeap,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));
}
