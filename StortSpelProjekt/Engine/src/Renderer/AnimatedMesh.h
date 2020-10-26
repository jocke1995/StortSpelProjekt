#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "Animation.h"
#include "Mesh.h"

// Forward declarations

struct VertexWeight
{
    unsigned int boneIDs[MAX_BONES_PER_VERTEX];
    float weights[MAX_BONES_PER_VERTEX];
};

class AnimatedMesh : public Mesh
{
public:
    AnimatedMesh(ID3D12Device5* device,
        std::vector<Vertex>* vertices,
        std::vector<VertexWeight>* vertexWeights,
        std::vector<unsigned int>* indices,
        std::map<unsigned int, VertexWeight>* perVertexBoneData,
        DescriptorHeap* descriptorHeap_SRV,
        const std::wstring& path = L"NOPATH");

    void Init(ID3D12Device5* m_pDevice5, DescriptorHeap* CBV_UAV_SRV_heap);

    const size_t GetSizeOfVertices() const;
    const size_t GetNumVertices() const;

    const void* GetVertexData() const;

    virtual ~AnimatedMesh();
private:
    std::vector<VertexWeight> m_VertexWeights;

};

#endif
