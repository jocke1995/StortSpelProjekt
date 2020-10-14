#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(ID3D12Device5* device, std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
	:Mesh(device, vertices, indices, descriptorHeap_SRV, path)
{
}

AnimatedMesh::~AnimatedMesh()
{
}
