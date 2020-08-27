#include "LightViewsPool.h"

LightViewsPool::LightViewsPool(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_CBV_UAV_SRV,
	DescriptorHeap* descriptorHeap_RTV,
	DescriptorHeap* descriptorHeap_DSV)
{
	this->device = device;

	this->descriptorHeap_CBV_UAV_SRV = descriptorHeap_CBV_UAV_SRV;
	this->descriptorHeap_RTV = descriptorHeap_RTV;
	this->descriptorHeap_DSV = descriptorHeap_DSV;
}

LightViewsPool::~LightViewsPool()
{
	for (int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE typeIndex = static_cast<LIGHT_TYPE>(i);

		// CBVs
		for (auto& pair : this->cbvPools[typeIndex])
		{
			delete pair.second;
		}

		// shadowInfos
		for (auto& tuple : this->shadowPools[typeIndex])
		{
			delete std::get<2>(tuple);
		}
	}
}

ConstantBufferView* LightViewsPool::GetFreeConstantBufferView(LIGHT_TYPE type)
{
	for (auto& pair : this->cbvPools[type])
	{
		// The resource is free
		if (pair.first == true)
		{
			pair.first = false;
			return pair.second;
		}
	}

	// No constant buffer of that type exists.. Create and return a new one
	ConstantBufferView* cbd = CreateConstantBufferView(type);
	this->cbvPools[type].push_back(std::make_pair(false, cbd));
	return cbd;
}

ShadowInfo* LightViewsPool::GetFreeShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution)
{
	// If there are a free shadowInfo, use it
	for (auto& tuple : this->shadowPools[lightType])
	{
		// The resource is free and the resolutions match
		if (std::get<0>(tuple) == true && std::get<1>(tuple) == shadowResolution)
		{
			std::get<0>(tuple) = false;
			return std::get<2>(tuple);
		}
	}
	
	// No shadowInfo of that type exists.. Create and return a new one
	ShadowInfo* si = this->CreateShadowInfo(lightType, shadowResolution);
	this->shadowPools[lightType].push_back(std::tuple(false, shadowResolution, si));
	return si;
}

void LightViewsPool::Clear()
{
	for (int i = 0; i < LIGHT_TYPE::NUM_LIGHT_TYPES; i++)
	{
		LIGHT_TYPE typeIndex = static_cast<LIGHT_TYPE>(i);

		// CBVs
		for (auto& pair : this->cbvPools[typeIndex])
		{
			pair.first = true;
		}

		// shadowInfos
		for (auto& tuple : this->shadowPools[typeIndex])
		{
			std::get<0>(tuple) = true;;
		}
	}
}

ConstantBufferView* LightViewsPool::CreateConstantBufferView(LIGHT_TYPE type)
{
	unsigned int entrySize = 0;
	std::wstring resourceName = L"";
	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		entrySize = (sizeof(DirectionalLight) + 255) & ~255;	// align to 255-byte boundary
		resourceName = L"DirectionalLight_DefaultResource";
		break;
	case LIGHT_TYPE::POINT_LIGHT:
		entrySize = (sizeof(PointLight) + 255) & ~255;	// align to 255-byte boundary
		resourceName = L"PointLight_DefaultResource";
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		entrySize = (sizeof(SpotLight) + 255) & ~255;	// align to 255-byte boundary
		resourceName = L"SpotLight_DefaultResource";
		break;
	}

	ConstantBufferView* cbd = new ConstantBufferView(
		device,
		entrySize,
		resourceName,
		this->descriptorHeap_CBV_UAV_SRV->GetNextDescriptorHeapIndex(1),
		this->descriptorHeap_CBV_UAV_SRV);

	return cbd;
}

ShadowInfo* LightViewsPool::CreateShadowInfo(LIGHT_TYPE lightType, SHADOW_RESOLUTION shadowResolution)
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
		this->shadowInfoIdCounter++,
		shadowResolution,
		this->device,
		this->descriptorHeap_DSV,
		this->descriptorHeap_CBV_UAV_SRV);

	return shadowInfo;
}
