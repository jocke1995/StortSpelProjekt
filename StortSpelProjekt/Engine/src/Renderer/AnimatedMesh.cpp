#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(ID3D12Device5* device, std::vector<AnimatedVertex>* vertices, std::vector<unsigned int>* indices, std::map<unsigned int, VertexWeight>* perVertexBoneData, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
	:Mesh(device, nullptr, indices, descriptorHeap_SRV, path)
{
	m_AnimatedVertices = *vertices;
}

AnimatedMesh::~AnimatedMesh()
{
}
