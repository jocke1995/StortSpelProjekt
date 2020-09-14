#ifndef PINGPONGRESOURCE_H
#define PINGPONGRESOURCE_H

class Resource;
class ShaderResourceView;
class UnorderedAccessView;
class DescriptorHeap;

// DX forward declarations
struct ID3D12Device5;
struct D3D12_SHADER_RESOURCE_VIEW_DESC;
struct D3D12_UNORDERED_ACCESS_VIEW_DESC;

class PingPongResource
{
public:
	PingPongResource(
		Resource* resource,
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
		D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);
	virtual ~PingPongResource();

	const Resource* const GetResource() const;
	const ShaderResourceView* const GetSRV() const;
	const UnorderedAccessView* const GetUAV() const;

private:
	Resource* m_pResource = nullptr;
	ShaderResourceView* m_pSRV = nullptr;
	UnorderedAccessView* m_pUAV = nullptr;
};

#endif
