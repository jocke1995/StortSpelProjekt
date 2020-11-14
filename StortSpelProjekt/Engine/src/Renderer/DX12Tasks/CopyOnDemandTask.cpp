#include "stdafx.h"
#include "CopyOnDemandTask.h"

#include "../Texture/Texture.h"
#include "../GPUMemory/Resource.h"
#include "../CommandInterface.h"

#include "../Texture/TextureCubeMap.h"

#include "../../ECS/Components/GUI2DComponent.h"
#include "../Renderer/Mesh.h"

CopyOnDemandTask::CopyOnDemandTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD)
	:CopyTask(device, interfaceType, FLAG_THREAD)
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

void CopyOnDemandTask::UnSubmitMesh(Mesh* mesh)
{
	// Erase uploaded data
	Resource* uploadResourceVertices = mesh->m_pUploadResourceVertices;
	Resource* uploadResourceIndices = mesh->m_pDefaultResourceIndices;
	auto it = m_UploadDefaultData.begin();

	// Remove vertexThings
	while (it != m_UploadDefaultData.end())
	{
		if (std::get<0>(*it) == uploadResourceVertices)
		{
			it = m_UploadDefaultData.erase(it);
			break;
		}
		it++;
	}

	// Remove indexThings
	it = m_UploadDefaultData.begin();
	while (it != m_UploadDefaultData.end())
	{
		if (std::get<1>(*it) == uploadResourceIndices)
		{
			it = m_UploadDefaultData.erase(it);
			break;
		}
		it++;
	}
}

void CopyOnDemandTask::UnSubmitText(Text* text)
{
	// Erase uploaded data
	Resource* uploadResource = text->m_pUploadResourceVertices;
	auto it = m_UploadDefaultData.begin();
	while (it != m_UploadDefaultData.end())
	{
		if (std::get<0>(*it) == uploadResource)
		{
			it = m_UploadDefaultData.erase(it);
			break;
		}
		else
		{
			++it;
		}
	}
}

void CopyOnDemandTask::Execute()
{
	ID3D12CommandAllocator* commandAllocator = m_pCommandInterface->GetCommandAllocator(m_CommandInterfaceIndex);
	ID3D12GraphicsCommandList5* commandList = m_pCommandInterface->GetCommandList(m_CommandInterfaceIndex);

	m_pCommandInterface->Reset(m_CommandInterfaceIndex);

	if (m_UploadDefaultData.size() > 0)
	{
		static int counter = 0;
		Log::Print("CopyOnDemand: %d runs with size: %d}\n", counter, m_UploadDefaultData.size());
		counter++;
	}

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

	// Transfer the data
	UpdateSubresources(commandList,
		defaultHeap, uploadHeap,
		0, 0, texture->m_SubresourceData.size(),
		texture->m_SubresourceData.data());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		defaultHeap,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));
}
