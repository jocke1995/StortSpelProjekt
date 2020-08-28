#ifndef RESOURCE_H
#define RESOURCE_H

#include <d3d12.h>

enum RESOURCE_TYPE
{
    UPLOAD,
    DEFAULT,
    NUM_RESOURCE_TYPES
};

class Resource
{
public:
    // Use for basic data
    Resource(
        ID3D12Device* device,
        unsigned long long entrySize,
        RESOURCE_TYPE type,
        std::wstring name = L"RESOURCE_NONAME");

    // Use for textures
    Resource(
        ID3D12Device* device,
        D3D12_RESOURCE_DESC* resourceDesc,
        D3D12_CLEAR_VALUE* clearValue = nullptr,
        std::wstring name = L"RESOURCE_NONAME",
        D3D12_RESOURCE_STATES startState = D3D12_RESOURCE_STATE_COMMON);

    // Default Constructor
    Resource() = default;
    virtual ~Resource();

    virtual unsigned int GetSize() const;
    ID3D12Resource1* GetID3D12Resource1() const;
    ID3D12Resource1** GetID3D12Resource1PP();
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAdress() const;

    // Only to be used if the resource is of type: UPLOAD
    void SetData(const void* data, unsigned int subResourceIndex = 0);
    // Only to be used if the resource is of type: UPLOAD
    void SetData(const void* data, unsigned int subResourceIndex = 0) const;
protected:
    unsigned long long entrySize = 0;
    RESOURCE_TYPE type;
    std::wstring name;

    D3D12_HEAP_PROPERTIES heapProperties = {};
    void SetupHeapProperties(D3D12_HEAP_TYPE heapType);

    ID3D12Resource1* resource = nullptr;
    void CreateResource(
        ID3D12Device* device,
        D3D12_RESOURCE_DESC* resourceDesc,
        D3D12_CLEAR_VALUE* clearValue,
        D3D12_RESOURCE_STATES startState);
};

#endif