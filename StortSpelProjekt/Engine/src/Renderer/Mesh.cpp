#include "stdafx.h"
#include "Mesh.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"
#include "Texture/Texture.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, const std::wstring& path)
{
	m_Path = path;
	m_Vertices = *vertices;
	m_Indices = *indices;
}

Mesh::~Mesh()
{
	if (m_pUploadResourceVertices != nullptr)
	{
		delete m_pUploadResourceVertices;
	}
	

	if (m_pDefaultResourceVertices != nullptr)
	{
		delete m_pDefaultResourceVertices;
	}
	

	// Set indices
	if (m_pUploadResourceIndices != nullptr)
	{
		delete m_pUploadResourceIndices;
	}
	
	if (m_pDefaultResourceIndices != nullptr)
	{
		delete m_pDefaultResourceIndices;
	}

	if (m_pSRV != nullptr)
	{
		delete m_pSRV;
	}

	if (m_pIndexBufferView != nullptr)
	{
		delete m_pIndexBufferView;
	}
}

void Mesh::Init(ID3D12Device5* m_pDevice5, DescriptorHeap* CBV_UAV_SRV_heap)
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
	dsrv.Buffer.StructureByteStride = sizeof(Vertex);

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


Resource* Mesh::GetDefaultResourceVertices() const
{
	return m_pDefaultResourceVertices;
}

const std::vector<Vertex>* Mesh::GetVertices() const
{
	return &m_Vertices;
}

const size_t Mesh::GetSizeOfVertices() const
{
	return m_Vertices.size() * sizeof(Vertex);
}

const size_t Mesh::GetNumVertices() const
{
	return m_Vertices.size();
}

const void* Mesh::GetVertexData() const
{
	return m_Vertices.data();
}

Resource* Mesh::GetDefaultResourceIndices() const
{
	return m_pDefaultResourceIndices;
}

const std::vector<unsigned int>* Mesh::GetIndices() const
{
	return &m_Indices;
}

const size_t Mesh::GetSizeOfIndices() const
{
	return m_Indices.size() * sizeof(unsigned int);
}

const size_t Mesh::GetNumIndices() const
{
	return m_Indices.size();
}

const D3D12_INDEX_BUFFER_VIEW* Mesh::GetIndexBufferView() const
{
	return m_pIndexBufferView;
}

const std::wstring& Mesh::GetPath() const
{
	return m_Path;
}

ShaderResourceView* const Mesh::GetSRV() const
{
	return m_pSRV;
}
