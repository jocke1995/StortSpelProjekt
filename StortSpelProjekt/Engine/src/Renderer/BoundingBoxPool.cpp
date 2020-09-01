#include "stdafx.h"
#include "BoundingBoxPool.h"

#include "DescriptorHeap.h"
#include "Mesh.h"

BoundingBoxPool::BoundingBoxPool(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	this->device = device;
	this->descriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
}

BoundingBoxPool::~BoundingBoxPool()
{
	for (auto& pair : this->boundingBoxesMesh)
	{
		delete pair.second;
	}

	for (auto& pair : this->boundingBoxesData)
	{
		delete pair.second;
	}
}

BoundingBoxPool* BoundingBoxPool::Get(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	static BoundingBoxPool instance(device, descriptorHeap_CBV_UAV_SRV);

	return &instance;
}

bool BoundingBoxPool::BoundingBoxDataExists(std::string uniquePath) const
{
	if (this->boundingBoxesData.count(uniquePath) != 0)
	{
		return true;
	}
	return false;
}

bool BoundingBoxPool::BoundingBoxMeshExists(std::string uniquePath) const
{
	if (this->boundingBoxesMesh.count(uniquePath) != 0)
	{
		return true;
	}
	return false;
}

BoundingBoxData* BoundingBoxPool::GetBoundingBoxData(std::string uniquePath)
{
	if (this->BoundingBoxDataExists(uniquePath) == true)
	{
		return this->boundingBoxesData.at(uniquePath);
	}
	return nullptr;
}

BoundingBoxData* BoundingBoxPool::CreateBoundingBoxData(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::string uniquePath)
{
	if (this->BoundingBoxDataExists(uniquePath) == false)
	{
		BoundingBoxData* bbd = new BoundingBoxData();
		bbd->boundingBoxVertices = vertices;
		bbd->boundingBoxIndices = indices;
		this->boundingBoxesData[uniquePath] = bbd;
	}
	return this->boundingBoxesData[uniquePath];
}

Mesh* BoundingBoxPool::CreateBoundingBoxMesh(std::string uniquePath)
{
	// If it already exists.. return it
	if (this->BoundingBoxMeshExists(uniquePath) == true)
	{
		return this->boundingBoxesMesh.at(uniquePath);
	}

	// else create it and return it if the data exists
	if (this->BoundingBoxDataExists(uniquePath) == true)
	{
		BoundingBoxData* bbd = this->boundingBoxesData[uniquePath];
		this->boundingBoxesMesh[uniquePath] = new Mesh(this->device, bbd->boundingBoxVertices, bbd->boundingBoxIndices, this->descriptorHeap_CBV_UAV_SRV, uniquePath);
		return this->boundingBoxesMesh[uniquePath];
	}

	// else return nullptr
	return nullptr;
}
