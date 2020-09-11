#ifndef BLOOM_H
#define BLOOM_H

class DescriptorHeap;
struct ID3D12Device5;
class RenderTarget;
class Resource;

class PingPongBuffer;

#include <array>
class BloomResources
{
public:
	BloomResources(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd);
	virtual ~BloomResources();

	const RenderTarget* const GetRenderTarget() const;
	
	const PingPongBuffer* GetPingPongBuffer(unsigned int index) const;

private:
	Resource* m_pResource = nullptr;
	RenderTarget* m_pRenderTarget = nullptr;

	// The compute shader will read and write in a "Ping-Pong"-order to these objects.
	std::array<PingPongBuffer*, 2> m_PingPongBuffers;
	

	void createBrightRenderTarget(
		ID3D12Device5* device,
		DescriptorHeap* dhRTV,
		unsigned int width, unsigned int height);
};

#endif
