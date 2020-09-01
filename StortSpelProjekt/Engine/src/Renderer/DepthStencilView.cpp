#include "stdafx.h"
#include "DepthStencilView.h"

#include "DescriptorHeap.h"
#include "Resource.h"

// Constructor that also creates a resource
DepthStencilView::DepthStencilView(
	ID3D12Device5* device,
	unsigned int width, unsigned int height,
	std::wstring dsvResourceName,
	DescriptorHeap* descriptorHeap_DSV,
	DXGI_FORMAT format)
{
	this->CreateResource(device, width, height, dsvResourceName, format);
	this->CreateDSV(device, descriptorHeap_DSV, format);
	this->dxgi_Format = format;

	this->deleteResource = true;
}

// Constructor that assigns a resource
DepthStencilView::DepthStencilView(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_DSV,
	Resource* resource,
	DXGI_FORMAT format)
{
	this->resource = resource;
	this->CreateDSV(device, descriptorHeap_DSV, format);
	this->dxgi_Format = format;

	this->deleteResource = false;
}

DepthStencilView::~DepthStencilView()
{
	if (this->deleteResource == true)
		delete this->resource;
}

Resource* DepthStencilView::GetDSVResource() const
{
	return this->resource;
}

unsigned int DepthStencilView::GetDescriptorHeapIndex() const
{
	return this->descriptorHeapIndex_DSV;
}

DXGI_FORMAT DepthStencilView::GetDXGIFormat() const
{
	return this->dxgi_Format;
}

void DepthStencilView::CreateResource(
	ID3D12Device5* device,
	unsigned int width, unsigned int height,
	std::wstring dsvResourceName,
	DXGI_FORMAT format)
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Format = format;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.DepthStencil.Depth = 1;
	clearValue.DepthStencil.Stencil = 0;

	this->resource = new Resource(
		device,
		&resourceDesc,
		&clearValue,
		dsvResourceName,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DepthStencilView::CreateDSV(
	ID3D12Device5* device,
	DescriptorHeap* descriptorHeap_DSV,
	DXGI_FORMAT format)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = format;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	this->descriptorHeapIndex_DSV = descriptorHeap_DSV->GetNextDescriptorHeapIndex(1);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = descriptorHeap_DSV->GetCPUHeapAt(descriptorHeapIndex_DSV);
	device->CreateDepthStencilView(this->resource->GetID3D12Resource1(), &depthStencilDesc, cdh);
}

