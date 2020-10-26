#include "AnimatedMesh.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "GPUMemory/UnorderedAccessView.h"
#include "DescriptorHeap.h"
#include "Texture/Texture.h"

AnimatedMesh::AnimatedMesh(ID3D12Device5* device, std::vector<Vertex>* vertices, std::vector<VertexWeight>* vertexWeights, std::vector<unsigned int>* indices, std::map<unsigned int, VertexWeight>* perVertexBoneData, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
	:Mesh(vertices, indices, path)
{
	m_VertexWeights = *vertexWeights;
}

AnimatedMesh::~AnimatedMesh()
{
	delete m_pUploadResourceOrigVertices;
	delete m_pDefaultResourceOrigVertices;

	delete m_pUploadResourceVertexWeights;
	delete m_pDefaultResourceVertexWeights;

	delete m_pSRVOrigVertices;
	delete m_pSRVVertexWeights;
	
	delete m_pUAV;
}

void AnimatedMesh::Init(ID3D12Device5* m_pDevice5, DescriptorHeap* CBV_UAV_SRV_heap)
{
	/* ------------------------- Normal vertex data ------------------------- */
	// create vertices resource
	m_pUploadResourceVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"AnimatedVertex_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"AnimatedVertex_DEFAULT_RESOURCE", D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = GetNumVertices();
	srvDesc.Buffer.StructureByteStride = sizeof(Vertex);

	// Set view to mesh
	m_pSRV = new ShaderResourceView(
		m_pDevice5,
		CBV_UAV_SRV_heap,
		&srvDesc,
		m_pDefaultResourceVertices);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = GetNumVertices();
	uavDesc.Buffer.StructureByteStride = sizeof(Vertex);

	m_pUAV = new UnorderedAccessView(
		m_pDevice5,
		CBV_UAV_SRV_heap,
		&uavDesc,
		m_pDefaultResourceVertices);

	// Set indices resource
	m_pUploadResourceIndices = new Resource(m_pDevice5, GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	m_pDefaultResourceIndices = new Resource(m_pDevice5, GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");

	// Set indexBufferView
	m_pIndexBufferView = new D3D12_INDEX_BUFFER_VIEW();
	m_pIndexBufferView->BufferLocation = m_pDefaultResourceIndices->GetGPUVirtualAdress();
	m_pIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
	m_pIndexBufferView->SizeInBytes = GetSizeOfIndices();
	/* ------------------------- Normal vertex data ------------------------- */



	/* ------------------------- To be animated vertex data ------------------------- */
	// create vertices resource
	m_pUploadResourceOrigVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"OrigVertex_UPLOAD_RESOURCE");
	m_pDefaultResourceOrigVertices = new Resource(m_pDevice5, GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"OrigVertex_DEFAULT_RESOURCE");

	// Set view to mesh
	m_pSRVOrigVertices = new ShaderResourceView(
		m_pDevice5,
		CBV_UAV_SRV_heap,
		&srvDesc,
		m_pDefaultResourceOrigVertices);

	// create vertices resource
	m_pUploadResourceVertexWeights = new Resource(m_pDevice5, m_VertexWeights.size() * sizeof(VertexWeight), RESOURCE_TYPE::UPLOAD, L"VertexWeight_UPLOAD_RESOURCE");
	m_pDefaultResourceVertexWeights = new Resource(m_pDevice5, m_VertexWeights.size() * sizeof(VertexWeight), RESOURCE_TYPE::DEFAULT, L"VertexWeight_DEFAULT_RESOURCE");

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescWeights = {};
	srvDescWeights.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDescWeights.Buffer.FirstElement = 0;
	srvDescWeights.Format = DXGI_FORMAT_UNKNOWN;
	srvDescWeights.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescWeights.Buffer.NumElements = m_VertexWeights.size();
	srvDescWeights.Buffer.StructureByteStride = sizeof(VertexWeight);

	// Set view to mesh
	m_pSRVVertexWeights = new ShaderResourceView(
		m_pDevice5,
		CBV_UAV_SRV_heap,
		&srvDescWeights,
		m_pDefaultResourceVertexWeights);
	/* ------------------------- To be animated vertex data ------------------------- */
}

Resource* AnimatedMesh::GetDefaultResourceOrigVertices() const
{
	return m_pDefaultResourceOrigVertices;
}

Resource* AnimatedMesh::GetDefaultResourceVertexWeights() const
{
	return m_pDefaultResourceVertexWeights;
}

const std::vector<VertexWeight>* AnimatedMesh::GetVertexWeights() const
{
	return &m_VertexWeights;
}

ShaderResourceView* AnimatedMesh::GetOrigVerticesSRV() const
{
	return m_pSRVOrigVertices;
}

ShaderResourceView* AnimatedMesh::GetVertexWeightSRV() const
{
	return m_pSRVVertexWeights;
}


UnorderedAccessView* AnimatedMesh::GetUAV() const
{
	return m_pUAV;
}
