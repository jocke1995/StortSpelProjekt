#ifndef BOUNDINGBOXPOOL_H
#define BOUNDINGBOXPOOL_H

#include "Core.h"

class DescriptorHeap;
class Mesh;
struct Vertex;
struct ID3D12Device5;

struct BoundingBoxData
{
	std::vector<Vertex> boundingBoxVertices;
	std::vector<unsigned int> boundingBoxIndices;
};

class BoundingBoxPool
{
public:
	~BoundingBoxPool();
	static BoundingBoxPool* Get(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);

	bool BoundingBoxDataExists(std::string uniquePath) const;
	bool BoundingBoxMeshExists(std::string uniquePath) const;

	BoundingBoxData* GetBoundingBoxData(std::string uniquePath);
	BoundingBoxData* CreateBoundingBoxData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string uniquePath);

	Mesh* CreateBoundingBoxMesh(std::string uniquePath);
	
private:
	BoundingBoxPool(ID3D12Device5* device = nullptr, DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr);
	BoundingBoxPool(BoundingBoxPool const&) = delete;
	void operator=(BoundingBoxPool const&) = delete;

	ID3D12Device5* m_pDevice = nullptr;
	DescriptorHeap* m_pDescriptorHeap_CBV_UAV_SRV = nullptr;

	std::map<std::string, BoundingBoxData*> m_BoundingBoxesData;
	std::map<std::string, Mesh*> m_BoundingBoxesMesh;

};

#endif
