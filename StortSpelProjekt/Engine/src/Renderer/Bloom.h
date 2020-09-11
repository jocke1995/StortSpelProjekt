#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
class Resource;

class PingPongResource;

#include <array>
class Bloom
{
public:
	Bloom(
		ID3D12Device5* device,
		DescriptorHeap* dhRTV, DescriptorHeap* dh_CBV_UAV_SRV,
		const HWND* hwnd);
	virtual ~Bloom();

	const RenderTarget* const GetRenderTarget() const;
	
	const PingPongResource* GetPingPongResource(unsigned int index) const;

private:
	RenderTarget* m_pRenderTarget = nullptr;

	std::array<Resource*, 2> m_resources;
	// The compute shader will read and write in a "Ping-Pong"-order to these objects.
	std::array<PingPongResource*, 2> m_PingPongResources;
	
	void createResources(ID3D12Device5* device, unsigned int width, unsigned int height);
	void createBrightRenderTarget(
		ID3D12Device5* device,
		DescriptorHeap* dhRTV,
		unsigned int width, unsigned int height);
};

#endif
