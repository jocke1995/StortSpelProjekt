#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "Animation.h"
#include "Mesh.h"

// Forward declarations

struct AnimatedVertex : public Vertex
{
    unsigned int boneIDs[MAX_BONES_PER_VERTEX];
    float weights[MAX_BONES_PER_VERTEX];

    AnimatedVertex operator=(Vertex vertex)
    {
        pos = vertex.pos;
        normal = vertex.normal;
        tangent = vertex.tangent;
        uv = vertex.uv;

        return *this;
    }
    AnimatedVertex operator=(VertexWeight vertexWeight)
    {
        for (unsigned int i = 0; i < MAX_BONES_PER_VERTEX; i++)
        {
            boneIDs[i] = vertexWeight.boneIDs[i];
            weights[i] = vertexWeight.weights[i];
        }

        return *this;
    }
};

class AnimatedMesh : public Mesh
{
public:
    AnimatedMesh(ID3D12Device5* device,
        std::vector<AnimatedVertex>* vertices,
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
    std::vector<AnimatedVertex> m_AnimatedVertices;

};

#endif
