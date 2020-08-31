#ifndef MESH_H
#define MESH_H

#include <d3d12.h>
#include "EngineMath.h"
#include "Resource.h"
#include "ShaderResourceView.h"
#include "Core.h"

#include "Material.h"

// temp
#include "CommandInterface.h"

struct Vertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 uv;
    DirectX::XMFLOAT4 normal;
    DirectX::XMFLOAT4 tangent;
};

class Mesh
{
public:
    Mesh(   ID3D12Device5* device,
            std::vector<Vertex> vertices,
            std::vector<unsigned int> indices,
            DescriptorHeap* descriptorHeap_SRV,
            const std::string path = "");
    Mesh(const Mesh* other);
    ~Mesh();

    // Vertices
    Resource* GetDefaultResourceVertices() const;
    const std::vector<Vertex>* GetVertices() const;
    const size_t GetSizeOfVertices() const;
    const size_t GetNumVertices() const;

    // Indices
    Resource* GetDefaultResourceIndices() const;
    const std::vector<unsigned int>* GetIndices() const;
    const size_t GetSizeOfIndices() const;
    const size_t GetNumIndices() const;
    const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() const;

    const SlotInfo* GetSlotInfo() const;
    std::string GetPath();
    Material* GetMaterial() const;

private:
    // Renderer needs to be able to use member variables of mesh without giving access to them outside of the engine
    friend class Renderer;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string path = "";

    Resource* uploadResourceVertices = nullptr;
    Resource* uploadResourceIndices = nullptr;
    Resource* defaultResourceVertices = nullptr;
    Resource* defaultResourceIndices = nullptr;

    ShaderResourceView* SRV = nullptr;

    // Material will write descriptorIndices to "slotinfo" in mesh 
    Material* material = nullptr;
    SlotInfo* slotInfo = nullptr;

    D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
    void CreateIndexBufferView();

    // Temporay solution to make sure each "new" mesh only gets deleted once
    bool isCopied = false;
};

#endif