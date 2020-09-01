#ifndef VIEWPOOL_H
#define VIEWPOOL_H

class ConstantBufferView;
class DescriptorHeap;
class ShadowInfo;
class Light;

#include "Core.h"

class ViewPool
{
public:
	ViewPool(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_DSV);
	~ViewPool();

	ConstantBufferView* GetFreeCBV(unsigned int size, std::wstring resourceName = L"CBV_DEFAULTNAME");
	ShadowInfo* GetFreeShadowInfo(LIGHT_TYPE type, SHADOW_RESOLUTION shadowResolution);

	void ClearAll();
	void ClearSpecificLight(LIGHT_TYPE type, ConstantBufferView* cbv, ShadowInfo* si);
	void ClearSpecificCBV(unsigned int size, ConstantBufferView* cbv);


private:
	ID3D12Device5* m_pDevice = nullptr;

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr;
	DescriptorHeap* descriptorHeap_RTV = nullptr;
	DescriptorHeap* descriptorHeap_DSV = nullptr;

	std::map<unsigned int, std::vector<std::pair<bool, ConstantBufferView*>>> cbvPool;
	ConstantBufferView* CreateConstantBufferView(unsigned int size, std::wstring resourceName);

	std::map<LIGHT_TYPE, std::vector<std::tuple<bool, SHADOW_RESOLUTION, ShadowInfo*>>> shadowPools;
	ShadowInfo* CreateShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution);
	unsigned int shadowInfoIdCounter = 0;
};

#endif
