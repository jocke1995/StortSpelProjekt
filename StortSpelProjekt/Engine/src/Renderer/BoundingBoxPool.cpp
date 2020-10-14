#include "stdafx.h"
#include "BoundingBoxPool.h"

#include "DescriptorHeap.h"
#include "Mesh.h"

BoundingBoxPool::BoundingBoxPool(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	m_pDevice = device;
	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
}

BoundingBoxPool::~BoundingBoxPool()
{
	for (auto& pair : m_BoundingBoxesMesh)
	{
		delete pair.second;
	}

	for (auto& pair : m_BoundingBoxesData)
	{
		delete pair.second;
	}
}

BoundingBoxPool* BoundingBoxPool::Get(ID3D12Device5* device, DescriptorHeap* descriptorHeap_CBV_UAV_SRV)
{
	static BoundingBoxPool instance(device, descriptorHeap_CBV_UAV_SRV);

	return &instance;
}

bool BoundingBoxPool::BoundingBoxDataExists(std::wstring uniquePath) const
{
	if (m_BoundingBoxesData.count(uniquePath) != 0)
	{
		return true;
	}
	return false;
}

bool BoundingBoxPool::BoundingBoxMeshExists(std::wstring uniquePath) const
{
	if (m_BoundingBoxesMesh.count(uniquePath) != 0)
	{
		return true;
	}
	return false;
}

BoundingBoxData* BoundingBoxPool::GetBoundingBoxData(std::wstring uniquePath)
{
	if (BoundingBoxDataExists(uniquePath) == true)
	{
		return m_BoundingBoxesData.at(uniquePath);
	}
	return nullptr;
}

BoundingBoxData* BoundingBoxPool::CreateBoundingBoxData(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::wstring uniquePath)
{
	if (BoundingBoxDataExists(uniquePath) == false)
	{
		BoundingBoxData* bbd = new BoundingBoxData();
		bbd->boundingBoxVertices = vertices;
		bbd->boundingBoxIndices = indices;
		m_BoundingBoxesData[uniquePath] = bbd;
	}
	return m_BoundingBoxesData[uniquePath];
}

Mesh* BoundingBoxPool::CreateBoundingBoxMesh(std::wstring uniquePath)
{
	// If it already exists.. return it
	if (BoundingBoxMeshExists(uniquePath) == true)
	{
		return m_BoundingBoxesMesh.at(uniquePath);
	}

	// else create it and return it if the data exists
	if (BoundingBoxDataExists(uniquePath) == true)
	{
		BoundingBoxData* bbd = m_BoundingBoxesData[uniquePath];
		m_BoundingBoxesMesh[uniquePath] = new Mesh(&bbd->boundingBoxVertices, &bbd->boundingBoxIndices, uniquePath);
		return m_BoundingBoxesMesh[uniquePath];
	}

	// else return nullptr
	return nullptr;
}
