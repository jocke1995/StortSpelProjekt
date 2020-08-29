#ifndef VIEWPOOL_H
#define VIEWPOOL_H

#include "ConstantBufferView.h"
//  SRV & DSV
#include "ShadowInfo.h"

#include "../ECS/Components/Lights/Light.h"

#include "../Headers/stdafx.h"

class ViewPool
{
public:
	ViewPool(
		ID3D12Device5* device,
		DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
		DescriptorHeap* descriptorHeap_RTV,
		DescriptorHeap* descriptorHeap_DSV);
	~ViewPool();

	ConstantBufferView* GetFreeCBV(unsigned int sizeAligned, std::wstring resourceName = L"CBV_DEFAULTNAME");
	ShadowInfo* GetFreeShadowInfo(LIGHT_TYPE type, SHADOW_RESOLUTION shadowResolution);

	void Clear();
	void ClearSpecificLight(LIGHT_TYPE type, ConstantBufferView* cbv, ShadowInfo* si);

private:
	ID3D12Device5* device = nullptr;

	DescriptorHeap* descriptorHeap_CBV_UAV_SRV = nullptr;
	DescriptorHeap* descriptorHeap_RTV = nullptr;
	DescriptorHeap* descriptorHeap_DSV = nullptr;

	std::map<unsigned int, std::vector<std::pair<bool, ConstantBufferView*>>> cbvPool;
	ConstantBufferView* CreateConstantBufferView(unsigned int sizeAligned, std::wstring resourceName);

	std::map<LIGHT_TYPE, std::vector<std::tuple<bool, SHADOW_RESOLUTION, ShadowInfo*>>> shadowPools;
	ShadowInfo* CreateShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution);
	unsigned int shadowInfoIdCounter = 0;
};

#endif
