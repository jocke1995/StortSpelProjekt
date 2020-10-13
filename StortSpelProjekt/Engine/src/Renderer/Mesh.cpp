#include "stdafx.h"
#include "Mesh.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"
#include "Texture/Texture.h"

Mesh::Mesh(ID3D12Device5* device, std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
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