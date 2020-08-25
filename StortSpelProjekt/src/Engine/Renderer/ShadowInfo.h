#ifndef SHADOWINFO_H
#define SHADOWINFO_H

#include "DepthStencilView.h"
#include "ShaderResourceView.h"
#include "RenderView.h"

enum SHADOW_RESOLUTION
{
	LOW,
	MEDIUM,
	HIGH,
	ULTRA,
	NUM_SHADOW_RESOLUTIONS,
	UNDEFINED
};

class ShadowInfo
{
public:
	ShadowInfo(
		unsigned int textureWidth, unsigned int textureHeight,
		unsigned int shadowInfoId,
		SHADOW_RESOLUTION shadowResolution,
		ID3D12Device5* device,
		DescriptorHeap* dh_DSV,
		DescriptorHeap* dh_SRV);

	virtual ~ShadowInfo();

	unsigned int GetId() const;
	SHADOW_RESOLUTION GetShadowResolution() const;
	Resource* GetResource() const;
	DepthStencilView* GetDSV() const;
	ShaderResourceView* GetSRV() const;
	RenderView* GetRenderView() const;

private:
	unsigned int id = 0;
	SHADOW_RESOLUTION shadowResolution = SHADOW_RESOLUTION::UNDEFINED;

	Resource* resource = nullptr;

	DepthStencilView* DSV = nullptr;
	ShaderResourceView* SRV = nullptr;

	RenderView* renderView = nullptr;
	
	void CreateResource(ID3D12Device5* device, unsigned int width, unsigned int height);
	void CreateDSV(ID3D12Device5* device, DescriptorHeap* dh_DSV);
	void CreateSRV(ID3D12Device5* device, DescriptorHeap* dh_SRV);
};

#endif