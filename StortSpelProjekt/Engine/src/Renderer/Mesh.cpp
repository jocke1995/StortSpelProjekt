#include "stdafx.h"
#include "Mesh.h"

#include "GPUMemory/Resource.h"
#include "GPUMemory/ShaderResourceView.h"
#include "DescriptorHeap.h"
#include "Texture.h"

Mesh::Mesh(	ID3D12Device5* device,
			std::vector<Vertex>* vertices,
			std::vector<unsigned int>* indices,
			DescriptorHeap* descriptorHeap_SRV,
			const std::string path)
{
	m_Path = path;
	
	m_Vertices = *vertices;
	m_Indices = *indices;

	initMesh(device, descriptorHeap_SRV);
}

Mesh::~Mesh()
{
	delete m_pUploadResourceVertices;

	delete m_pDefaultResourceVertices;

	// Set indices
	delete m_pUploadResourceIndices;
	delete m_pDefaultResourceIndices;

	delete m_pSRV;
	delete m_pIndexBufferView;
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

std::string Mesh::GetPath()
{
	return m_Path;
}

void Mesh::initMesh(ID3D12Device5* device, DescriptorHeap* descriptorHeap_SRV)
{
	// Set vertices
	m_pUploadResourceVertices = new Resource(device, GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");
	m_pDefaultResourceVertices = new Resource(device, GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

	// Set indices
	m_pUploadResourceIndices = new Resource(device, GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	m_pDefaultResourceIndices = new Resource(device, GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");
	createIndexBufferView();

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = GetNumVertices();
	dsrv.Buffer.StructureByteStride = sizeof(Vertex);

	m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap_SRV,
		&dsrv,
		m_pDefaultResourceVertices);
}

void Mesh::createIndexBufferView()
{
	m_pIndexBufferView = new D3D12_INDEX_BUFFER_VIEW();
	m_pIndexBufferView->BufferLocation = m_pDefaultResourceIndices->GetGPUVirtualAdress();
	m_pIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
	m_pIndexBufferView->SizeInBytes = GetSizeOfIndices();
}

