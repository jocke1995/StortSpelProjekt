#include "stdafx.h"
#include "Mesh.h"

#include "Resource.h"
#include "ShaderResourceView.h"
#include "Material.h"
#include "DescriptorHeap.h"

Mesh::Mesh(	ID3D12Device5* device,
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			DescriptorHeap* descriptorHeap_SRV,
			const std::string path)
{
	this->m_Vertices = vertices;
	this->m_Indices = indices;

	this->m_Path = path;

	// Set vertices
	this->m_pUploadResourceVertices = new Resource(device, this->GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");

	this->m_pDefaultResourceVertices = new Resource(device, this->GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

	// Set indices
	this->m_pUploadResourceIndices = new Resource(device, this->GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	this->m_pDefaultResourceIndices = new Resource(device, this->GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");
	this->createIndexBufferView();

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = this->GetNumVertices();
	dsrv.Buffer.StructureByteStride = sizeof(Vertex);

	this->m_pSRV = new ShaderResourceView(
		device,
		descriptorHeap_SRV,
		&dsrv,
		this->m_pDefaultResourceVertices);

	this->m_pSlotInfo = new SlotInfo();
	this->m_pSlotInfo->vertexDataIndex = this->m_pSRV->GetDescriptorHeapIndex();
	this->m_pMaterial = new Material(this->m_pSlotInfo);
}

Mesh::Mesh(const Mesh* other)
{
	this->m_IsCopied = true;

	// TODO: use the same vertices instead of copy
	this->m_Vertices = other->m_Vertices;
	this->m_Indices = other->m_Indices;
	this->m_Path = other->m_Path;

	this->m_pSlotInfo = new SlotInfo();
	this->m_pSlotInfo->vertexDataIndex = other->m_pSlotInfo->vertexDataIndex;

	// Set material properties
	Material* mat = other->GetMaterial();
	this->m_pMaterial = new Material(mat, this->m_pSlotInfo);
	for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		this->m_pMaterial->SetTexture(type, mat->GetTexture(type));
	}

	this->m_pUploadResourceVertices = other->m_pUploadResourceVertices;
	this->m_pDefaultResourceVertices = other->m_pDefaultResourceVertices;

	this->m_pUploadResourceIndices = other->m_pUploadResourceIndices;
	this->m_pDefaultResourceIndices = other->m_pDefaultResourceIndices;

	this->m_pIndexBufferView = other->m_pIndexBufferView;

	this->m_pSRV = other->m_pSRV;
}

Mesh::~Mesh()
{
	delete this->m_pSlotInfo;
	delete this->m_pMaterial;

	if (this->m_IsCopied == false)
	{
		delete this->m_pUploadResourceVertices;
		delete this->m_pDefaultResourceVertices;

		delete this->m_pUploadResourceIndices;
		delete this->m_pDefaultResourceIndices;

		delete this->m_pSRV;
		delete this->m_pIndexBufferView;
	}
}

Resource* Mesh::GetDefaultResourceVertices() const
{
	return this->m_pDefaultResourceVertices;
}

const std::vector<Vertex>* Mesh::GetVertices() const
{
	return &this->m_Vertices;
}

const size_t Mesh::GetSizeOfVertices() const
{
	return this->m_Vertices.size() * sizeof(Vertex);
}

const size_t Mesh::GetNumVertices() const
{
	return this->m_Vertices.size();
}

Resource* Mesh::GetDefaultResourceIndices() const
{
	return this->m_pDefaultResourceIndices;
}

const std::vector<unsigned int>* Mesh::GetIndices() const
{
	return &this->m_Indices;
}

const size_t Mesh::GetSizeOfIndices() const
{
	return this->m_Indices.size() * sizeof(unsigned int);
}

const size_t Mesh::GetNumIndices() const
{
	return this->m_Indices.size();
}

const D3D12_INDEX_BUFFER_VIEW* Mesh::GetIndexBufferView() const
{
	return this->m_pIndexBufferView;
}

const SlotInfo* Mesh::GetSlotInfo() const
{
	return this->m_pSlotInfo;
}

std::string Mesh::GetPath()
{
	return this->m_Path;
}

Material* Mesh::GetMaterial() const
{
	return this->m_pMaterial;
}

void Mesh::createIndexBufferView()
{
	this->m_pIndexBufferView = new D3D12_INDEX_BUFFER_VIEW();
	this->m_pIndexBufferView->BufferLocation = this->m_pDefaultResourceIndices->GetGPUVirtualAdress();
	this->m_pIndexBufferView->Format = DXGI_FORMAT_R32_UINT;
	this->m_pIndexBufferView->SizeInBytes = this->GetSizeOfIndices();
}

