#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(ID3D12Device5* device, std::vector<AnimatedVertex>* vertices, std::vector<unsigned int>* indices, std::map<unsigned int, VertexWeight>* perVertexBoneData, DescriptorHeap* descriptorHeap_SRV, const std::wstring& path)
	:Mesh(nullptr, indices, path)
{
	m_AnimatedVertices = *vertices;
}

AnimatedMesh::~AnimatedMesh()
{
}
