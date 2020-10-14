#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "Animation.h"
#include "Mesh.h"

// Forward declarations

struct AnimatedVertex : public Vertex
{
    unsigned int boneIDs[MAX_BONES_PER_VERTEX];
    float weights[MAX_BONES_PER_VERTEX];
};

class AnimatedMesh : public Mesh
{
public:
    AnimatedMesh(ID3D12Device5* device,
        std::vector<Vertex>* vertices,
        std::vector<unsigned int>* indices,
        DescriptorHeap* descriptorHeap_SRV,
        const std::wstring& path = L"NOPATH");
    virtual ~AnimatedMesh();
private:

};









#endif
