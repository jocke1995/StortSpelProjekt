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
	this->uploadResourceVertices->SetData(this->vertices.data());

	this->defaultResourceVertices = new Resource(device, this->GetSizeOfVertices(), RESOURCE_TYPE::DEFAULT, L"Vertex_DEFAULT_RESOURCE");

	// Set indices
	this->uploadResourceIndices = new Resource(device, this->GetSizeOfIndices(), RESOURCE_TYPE::UPLOAD, L"Index_UPLOAD_RESOURCE");
	this->uploadResourceIndices->SetData(this->indices.data());
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
	this->slotInfo->textureAmbient  = other->slotInfo->textureAmbient;
	this->slotInfo->textureDiffuse  = other->slotInfo->textureDiffuse;
	this->slotInfo->textureSpecular = other->slotInfo->textureSpecular;
	this->slotInfo->textureNormal   = other->slotInfo->textureNormal;
	this->slotInfo->textureEmissive = other->slotInfo->textureEmissive;

	this->defaultResourceVertices = other->defaultResourceVertices;
	this->defaultResourceIndices = other->defaultResourceIndices;

	this->indexBufferView = other->indexBufferView;

	this->SRV = other->SRV;
}

Mesh::~Mesh()
{
	delete this->slotInfo;

	if (this->isCopied == false)
	{
		delete this->uploadResourceVertices;
		delete this->defaultResourceVertices;

		delete this->uploadResourceIndices;
		delete this->defaultResourceIndices;

		delete this->SRV;
	}
}

void Mesh::UploadToDefault(ID3D12Device5* device, CommandInterface* commandInterface, ID3D12CommandQueue* cmdQueue)
{
	commandInterface->Reset(0);
	ID3D12GraphicsCommandList5* commandList = commandInterface->GetCommandList(0);

	/* ------------------------------------- Vertices ------------------------------------- */
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		this->defaultResourceVertices->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST));

	// To Defaultheap from Uploadheap
	commandList->CopyResource(
		this->defaultResourceVertices->GetID3D12Resource1(),	// Receiever
		this->uploadResourceVertices->GetID3D12Resource1());	// Sender

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		this->defaultResourceVertices->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));

	/* ------------------------------------- Indices ------------------------------------- */
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		this->defaultResourceIndices->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST));

	// To Defaultheap from Uploadheap
	commandList->CopyResource(
		this->defaultResourceIndices->GetID3D12Resource1(),	// Receiever
		this->uploadResourceIndices->GetID3D12Resource1());	// Sender

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		this->defaultResourceIndices->GetID3D12Resource1(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON));

	commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { commandList };
	cmdQueue->ExecuteCommandLists(ARRAYSIZE(ppCommandLists), ppCommandLists);
}

void Mesh::SetTexture(TEXTURE_TYPE textureType, Texture* texture)
{
	this->textures[textureType] = texture;
	
	switch (textureType)
	{
	case TEXTURE_TYPE::AMBIENT:
		this->slotInfo->textureAmbient = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::DIFFUSE:
		this->slotInfo->textureDiffuse = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::SPECULAR:
		this->slotInfo->textureSpecular = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::NORMAL:
		this->slotInfo->textureNormal = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::EMISSIVE:
		this->slotInfo->textureEmissive = texture->GetDescriptorHeapIndex();
		break;
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

Texture* Mesh::GetTexture(TEXTURE_TYPE textureType)
{
	return this->textures[textureType];
}

std::string Mesh::GetPath()
{
	return this->path;
}

void Mesh::CreateIndexBufferView()
{
	this->indexBufferView.BufferLocation = this->defaultResourceIndices->GetGPUVirtualAdress();
	this->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	this->indexBufferView.SizeInBytes = this->GetSizeOfIndices();
}

