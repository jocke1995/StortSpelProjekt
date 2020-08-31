#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(	ID3D12Device5* device,
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			DescriptorHeap* descriptorHeap_SRV,
			const std::string path)
{
	this->vertices = vertices;
	this->indices = indices;

	this->path = path;

	// Set vertices
	this->uploadResourceVertices = new Resource(device, this->GetSizeOfVertices(), RESOURCE_TYPE::UPLOAD, L"Vertex_UPLOAD_RESOURCE");

	this->defaultResourceVertices = new Resource(device, this->GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

	// Set indices
	this->uploadResourceIndices = new Resource(device, this->GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	this->defaultResourceIndices = new Resource(device, this->GetSizeOfIndices(), RESOURCE_TYPE::DEFAULT, L"Index_DEFAULT_RESOURCE");
	this->CreateIndexBufferView();

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC dsrv = {};
	dsrv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	dsrv.Buffer.FirstElement = 0;
	dsrv.Format = DXGI_FORMAT_UNKNOWN;
	dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dsrv.Buffer.NumElements = this->GetNumVertices();
	dsrv.Buffer.StructureByteStride = sizeof(Vertex);

	this->SRV = new ShaderResourceView(
		device,
		descriptorHeap_SRV,
		&dsrv,
		this->defaultResourceVertices);

	this->slotInfo = new SlotInfo();
	this->slotInfo->vertexDataIndex = this->SRV->GetDescriptorHeapIndex();
	this->material = new Material(this->slotInfo);
}

Mesh::Mesh(const Mesh* other)
{
	this->isCopied = true;

	// TODO: use the same vertices instead of copy
	this->vertices = other->vertices;
	this->indices = other->indices;
	this->path = other->path;

	this->slotInfo = new SlotInfo();
	this->slotInfo->vertexDataIndex = other->slotInfo->vertexDataIndex;

	// Set material properties
	Material* mat = other->GetMaterial();
	this->material = new Material(mat, this->slotInfo);
	for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		this->material->SetTexture(type, mat->GetTexture(type));
	}

	this->uploadResourceVertices = other->uploadResourceVertices;
	this->defaultResourceVertices = other->defaultResourceVertices;

	this->uploadResourceIndices = other->uploadResourceIndices;
	this->defaultResourceIndices = other->defaultResourceIndices;

	this->indexBufferView = other->indexBufferView;

	this->SRV = other->SRV;
}

Mesh::~Mesh()
{
	delete this->slotInfo;
	delete this->material;

	if (this->isCopied == false)
	{
		delete this->uploadResourceVertices;
		delete this->defaultResourceVertices;

		delete this->uploadResourceIndices;
		delete this->defaultResourceIndices;

		

		delete this->SRV;
	}
}

Resource* Mesh::GetDefaultResourceVertices() const
{
	return this->defaultResourceVertices;
}

const std::vector<Vertex>* Mesh::GetVertices() const
{
	return &this->vertices;
}

const size_t Mesh::GetSizeOfVertices() const
{
	return this->vertices.size() * sizeof(Vertex);
}

const size_t Mesh::GetNumVertices() const
{
	return this->vertices.size();
}

Resource* Mesh::GetDefaultResourceIndices() const
{
	return this->defaultResourceIndices;
}

const std::vector<unsigned int>* Mesh::GetIndices() const
{
	return &this->indices;
}

const size_t Mesh::GetSizeOfIndices() const
{
	return this->indices.size() * sizeof(unsigned int);
}

const size_t Mesh::GetNumIndices() const
{
	return this->indices.size();
}

const D3D12_INDEX_BUFFER_VIEW* Mesh::GetIndexBufferView() const
{
	return &this->indexBufferView;
}

const SlotInfo* Mesh::GetSlotInfo() const
{
	return this->slotInfo;
}

std::string Mesh::GetPath()
{
	return this->path;
}

Material* Mesh::GetMaterial() const
{
	return this->material;
}

void Mesh::CreateIndexBufferView()
{
	this->indexBufferView.BufferLocation = this->defaultResourceIndices->GetGPUVirtualAdress();
	this->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	this->indexBufferView.SizeInBytes = this->GetSizeOfIndices();
}

