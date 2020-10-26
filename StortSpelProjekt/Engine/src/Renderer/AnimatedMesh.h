#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "Animation.h"
#include "Mesh.h"

// Forward declarations
class UnorderedAccessView;
class ShaderResourceView;

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
    virtual ~AnimatedMesh();


    void Init(ID3D12Device5* m_pDevice5, DescriptorHeap* CBV_UAV_SRV_heap);

    // Data used for animation (base vertices + base vertex weights)
    Resource* GetDefaultResourceOrigVertices() const;
    Resource* GetDefaultResourceVertexWeights() const;
    const std::vector<VertexWeight>* GetVertexWeights() const;

    ShaderResourceView* GetOrigVerticesSRV() const;
    ShaderResourceView* GetVertexWeightSRV() const;

    // AnimatedMeshData
    UnorderedAccessView* GetUAV() const;

private:
    std::vector<VertexWeight> m_VertexWeights;

    Resource* m_pUploadResourceOrigVertices = nullptr;
    Resource* m_pDefaultResourceOrigVertices = nullptr;
    Resource* m_pUploadResourceVertexWeights = nullptr;
    Resource* m_pDefaultResourceVertexWeights = nullptr;

    ShaderResourceView* m_pSRVOrigVertices = nullptr;
    ShaderResourceView* m_pSRVVertexWeights = nullptr;

    // AnimatedMeshData, pointer to "mesh" base class data
    UnorderedAccessView* m_pUAV = nullptr;
};

#endif
