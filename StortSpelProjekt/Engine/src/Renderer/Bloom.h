#ifndef BLOOM_H
#define BLOOM_H

class SwapChain;
class DescriptorHeap;

class ShaderResourceView;
class RenderTargetView;
class Resource;

class PingPongResource;

struct ID3D12Device5;

#include <array>
class Bloom
{
public:
	Bloom(
		ID3D12Device5* device,
		DescriptorHeap* dhRTV, DescriptorHeap* dh_CBV_UAV_SRV,
		SwapChain* swapChain);
	virtual ~Bloom();

	const std::tuple<Resource*, RenderTargetView*, ShaderResourceView*>* GetBrightTuple() const;
	
	const PingPongResource* GetPingPongResource(unsigned int index) const;

private:
	std::tuple<Resource*, RenderTargetView*, ShaderResourceView*> m_BrightTuple;

	// The compute shader will read and write in a "Ping-Pong"-order to these objects.
	std::array<Resource*, 2> m_Resources;
	std::array<PingPongResource*, 2> m_PingPongResources;
	
	void createResources(ID3D12Device5* device, unsigned int width, unsigned int height);
	void createBrightTuple(
		ID3D12Device5* device,
		DescriptorHeap* dhRTV,
		unsigned int width, unsigned int height);
};

#endif
