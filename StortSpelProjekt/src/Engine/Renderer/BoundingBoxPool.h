#ifndef BOUNDINGBOXPOOL_H
#define BOUNDINGBOXPOOL_H

#include "Mesh.h"

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

	ID3D12Device5* device = nullptr;
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr;

	std::map<std::string, BoundingBoxData*> boundingBoxesData;
	std::map<std::string, Mesh*> boundingBoxesMesh;

};

#endif
