#include "stdafx.h"
#include "ViewPool.h"

#include "GPUMemory/ConstantBuffer.h"
#include "ShadowInfo.h"
#include "DescriptorHeap.h"
#include "../ECS/Components/Lights/Light.h"

ViewPool::ViewPool(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_DSV)
{
	m_pDevice = device;

	m_pDescriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
	m_pDescriptorHeap_RTV = descriptorHeap_RTV;
	m_pDescriptorHeap_DSV = descriptorHeap_DSV;
}

ViewPool::~ViewPool()
{
	// Free cbs
	for (auto& pair : m_CbPool)
	{
		for (auto& pair2 : pair.second)
		{
			// free memory of cb
			delete pair2.second;
		}
	}

	// Free shadowInfo
	for (unsigned int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE type = static_cast<LIGHT_TYPE>(i);
		for (auto& tuple : m_ShadowPools[type])
		{
			// free memory of shadowInfo
			delete std::get<2>(tuple);
		}
	}
}

ConstantBuffer* ViewPool::GetFreeCBV(unsigned int size, std::wstring resourceName)
{
	unsigned int sizeAligned = (size + 255) & ~255;

	for (auto& pair : m_CbPool[sizeAligned])
	{
		// The resource is free
		if (pair.first == true)
		{
			pair.first = false;
			return pair.second;
		}
	}

	// No constant buffer of that type exists.. Create and return a new one
	ConstantBuffer* cb = createConstantBuffer(sizeAligned, resourceName);
	m_CbPool[sizeAligned].push_back(std::make_pair(false, cb));
	return cb;
}

ShadowInfo* ViewPool::GetFreeShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution)
{
	// If there are a free shadowInfo, use it
	for (auto& tuple : m_ShadowPools[lightType])
	{
		// The resource is free and the resolutions match
		if (std::get<0>(tuple) == true && std::get<1>(tuple) == shadowResolution)
		{
			std::get<0>(tuple) = false;
			return std::get<2>(tuple);
		}
	}
	
	// No shadowInfo of that type exists.. Create and return a new one
	ShadowInfo* si = createShadowInfo(lightType, shadowResolution);
	m_ShadowPools[lightType].push_back(std::tuple(false, shadowResolution, si));
	return si;
}

void ViewPool::ClearAll()
{
	for (auto& pair : m_CbPool)
	{
		for (auto& vec : pair.second)
		{
			vec.first = true;
		}
	}

	for (int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE typeIndex = static_cast<LIGHT_TYPE>(i);
	
		// shadowInfos
		for (auto& tuple : m_ShadowPools[typeIndex])
		{
			std::get<0>(tuple) = true;;
		}
	}
}

void ViewPool::ClearSpecificLight(LIGHT_TYPE type, ConstantBuffer* cbv, ShadowInfo* si)
{
	unsigned int sizeAligned = 0;
	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		sizeAligned = (sizeof(DirectionalLight) + 255) & ~255;
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		sizeAligned = (sizeof(PointLight) + 255) & ~255;
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		sizeAligned = (sizeof(SpotLight) + 255) & ~255;
		break;
	}

	// Free cbv
	for (auto& pair : m_CbPool[sizeAligned])
	{
		if (pair.second == cbv)
		{
			pair.first = true;
			break;
		}
	}

	// Free shadowInfo
	if (si != nullptr)
	{
		for (auto& tuple : m_ShadowPools[type])
		{
			if (std::get<2>(tuple) == si)
			{
				std::get<0>(tuple) = true;
				break;
			}
		}
	}
}

void ViewPool::ClearSpecificCB(unsigned int size, ConstantBuffer* cbv)
{
	unsigned int sizeAligned = (size + 255) & ~255;
}

ConstantBuffer* ViewPool::createConstantBuffer(unsigned int size, std::wstring resourceName)
{
	unsigned int sizeAligned = (size + 255) & ~255;

	ConstantBuffer* cbd = new ConstantBuffer(
		m_pDevice,
		sizeAligned,
		resourceName,
		m_pDescriptorHeap_CBV_UAV_SRV);

	return cbd;
}

ShadowInfo* ViewPool::createShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution)
{
	
	unsigned int depthTextureWidth = 0;
	unsigned int depthTextureHeight = 0;
	switch (lightType)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		switch (shadowResolution)
		{
		case SHADOW_RESOLUTION::LOW:
			depthTextureWidth = 512;
			depthTextureHeight = 512;
			break;
		case SHADOW_RESOLUTION::MEDIUM:
			depthTextureWidth = 1024;
			depthTextureHeight = 1024;
			break;
		case SHADOW_RESOLUTION::HIGH:
			depthTextureWidth = 2048;
			depthTextureHeight = 2048;
			break;
		case SHADOW_RESOLUTION::ULTRA:
			depthTextureWidth = 4096;
			depthTextureHeight = 4096;
			break;
		}
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		switch (shadowResolution)
		{
		case SHADOW_RESOLUTION::LOW:
			depthTextureWidth = 256;
			depthTextureHeight = 256;
			break;
		case SHADOW_RESOLUTION::MEDIUM:
			depthTextureWidth = 512;
			depthTextureHeight = 512;
			break;
		case SHADOW_RESOLUTION::HIGH:
			depthTextureWidth = 1024;
			depthTextureHeight = 1024;
			break;
		case SHADOW_RESOLUTION::ULTRA:
			depthTextureWidth = 2048;
			depthTextureHeight = 2048;
			break;
		}
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		switch (shadowResolution)
		{
		case SHADOW_RESOLUTION::LOW:
			depthTextureWidth = 512;
			depthTextureHeight = 512;
			break;
		case SHADOW_RESOLUTION::MEDIUM:
			depthTextureWidth = 1024;
			depthTextureHeight = 1024;
			break;
		case SHADOW_RESOLUTION::HIGH:
			depthTextureWidth = 2048;
			depthTextureHeight = 2048;
			break;
		case SHADOW_RESOLUTION::ULTRA:
			depthTextureWidth = 4096;
			depthTextureHeight = 4096;
			break;
		}
		break;
	}


	ShadowInfo* shadowInfo = new ShadowInfo(
		depthTextureWidth,
		depthTextureHeight,
		m_ShadowInfoIdCounter++,
		shadowResolution,
		m_pDevice,
		m_pDescriptorHeap_DSV,
		m_pDescriptorHeap_CBV_UAV_SRV);

	return shadowInfo;
}
