#ifndef MESH_H
#define MESH_H

#include "EngineMath.h"
#include "Core.h"

class Resource;
class ShaderResourceView;
class Material;
class DescriptorHeap;
struct SlotInfo;

// DX12 Forward Declarations
struct ID3D12Device5;
struct D3D12_INDEX_BUFFER_VIEW;

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
    friend class Renderer;
    friend class SceneManager;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string path = "";

    Resource* uploadResourceVertices = nullptr;
    Resource* uploadResourceIndices = nullptr;
    Resource* defaultResourceVertices = nullptr;
    Resource* defaultResourceIndices = nullptr;

    ShaderResourceView* SRV = nullptr;

    // Material will write descriptorIndices to "slotinfo" in m_pMesh 
    Material* material = nullptr;
    SlotInfo* slotInfo = nullptr;

    D3D12_INDEX_BUFFER_VIEW* indexBufferView = nullptr;;
    void CreateIndexBufferView();

    // Temporay solution to make sure each "new" m_pMesh only gets deleted once
    bool isCopied = false;
};

#endif