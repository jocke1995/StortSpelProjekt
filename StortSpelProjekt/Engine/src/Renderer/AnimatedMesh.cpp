#include "AnimatedMesh.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"
#include "Texture/Texture.h"

AnimatedMesh::AnimatedMesh(ID3D12Device5* device, std::vector<Vertex>* vertices, std::vector<VertexWeight>* vertexWeights, std::vector<unsigned int>* indices, std::map<unsigned int, VertexWeight>* perVertexBoneData, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
	:Mesh(vertices, indices, path)
{
	m_VertexWeights = *vertexWeights;
}

void AnimatedMesh::Init(ID3D12Device5* m_pDevice5, DescriptorHeap* CBV_UAV_SRV_heap)
{
	// create vertices resource
	m_pUploadResourceVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = GetNumVertices();
	dsrv.Buffer.StructureByteStride = sizeof(VertexWeight);

	// Set view to mesh
	m_pSRV = new ShaderResourceView(
		m_pDevice5,
		CBV_UAV_SRV_heap,
		&dsrv,
		m_pDefaultResourceVertices);

	// Set indices resource
	m_pUploadResourceIndices = new Resource(m_pDevice5, GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	m_pDefaultResourceIndices = new Resource(m_pDevice5, GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");

	// Set indexBufferView
	m_pIndexBufferView = new D3D12_INDEX_BUFFER_VIEW();
	m_pIndexBufferView->BufferLocation = m_pDefaultResourceIndices->GetGPUVirtualAdress();
	m_pIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
	m_pIndexBufferView->SizeInBytes = GetSizeOfIndices();
}

const size_t AnimatedMesh::GetSizeOfVertices() const
{
	return m_VertexWeights.size() * sizeof(VertexWeight);
}

const size_t AnimatedMesh::GetNumVertices() const
{
	return  m_VertexWeights.size();
}

const void* AnimatedMesh::GetVertexData() const
{
	return m_VertexWeights.data();
}

AnimatedMesh::~AnimatedMesh()
{
}
