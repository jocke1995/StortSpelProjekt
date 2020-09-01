#include "stdafx.h"
#include "ShadowInfo.h"

#include "DepthStencilView.h"
#include "ShaderResourceView.h"
#include "RenderView.h"
#include "DescriptorHeap.h"
#include "Resource.h"

ShadowInfo::ShadowInfo(
	unsigned int textureWidth, unsigned int textureHeight,
	unsigned int shadowInfoId,
	SHADOW_RESOLUTION shadowResolution,
	ID3D12Device5* device,
	DescriptorHeap* dh_DSV,
	DescriptorHeap* dh_SRV)
{
	this->m_Id = shadowInfoId;
	this->m_ShadowResolution = shadowResolution;

	this->createResource(device, textureWidth, textureHeight);

	this->createDSV(device, dh_DSV);
	this->createSRV(device, dh_SRV);

	this->m_pRenderView = new RenderView(textureWidth, textureHeight);

}

bool ShadowInfo::operator==(const ShadowInfo& other)
{
	return this->m_Id == other.m_Id;
}

ShadowInfo::~ShadowInfo()
{
	delete this->m_pResource;

	delete this->m_pDSV;
	delete this->m_pSRV;

	delete this->m_pRenderView;
}

unsigned int ShadowInfo::GetId() const
{
	return this->m_Id;
}

SHADOW_RESOLUTION ShadowInfo::GetShadowResolution() const
{
	return this->m_ShadowResolution;
}

Resource* ShadowInfo::GetResource() const
{
	return this->m_pResource;
}

DepthStencilView* ShadowInfo::GetDSV() const
{
	return this->m_pDSV;
}

ShaderResourceView* ShadowInfo::GetSRV() const
{
	return this->m_pSRV;
}

RenderView* ShadowInfo::GetRenderView() const
{
	return this->m_pRenderView;
}

void ShadowInfo::createResource(ID3D12Device5* device, unsigned int width, unsigned int height)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Alignment = 0;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	std::wstring resourceName = L"ShadowMap" + std::to_wstring(this->m_Id) + L"_DEFAULT_RESOURCE";
	this->m_pResource = new Resource(
		device,
		&desc,
		&clearValue,
		resourceName,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ShadowInfo::createDSV(ID3D12Device5* device, DescriptorHeap* dh_DSV)
{
	this->m_pDSV = new DepthStencilView(
		device,
		dh_DSV,
		this->m_pResource,
		DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void ShadowInfo::createSRV(ID3D12Device5* device, DescriptorHeap* dh_SRV)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.ResourceMinLODClamp = 0.0f;
	srvd.Texture2D.PlaneSlice = 0;

	this->m_pSRV = new ShaderResourceView(
		device,
		dh_SRV,
		&srvd,
		this->m_pResource);
}
