#include "stdafx.h"
#include "RenderView.h"

RenderView::RenderView(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	this->CreateViewport();
	this->CreateScissorRect();
}

RenderView::~RenderView()
{
}

const D3D12_VIEWPORT* RenderView::GetViewPort() const
{
	return &this->viewport;
}

const D3D12_RECT* RenderView::GetScissorRect() const
{
	return &this->scissorRect;
}

void RenderView::CreateViewport()
{
	this->viewport.TopLeftX = 0.0f;
	this->viewport.TopLeftY = 0.0f;
	this->viewport.Width = this->width;
	this->viewport.Height = this->height;
	this->viewport.MinDepth = 0.0f;
	this->viewport.MaxDepth = 1.0f;
}

void RenderView::CreateScissorRect()
{
	this->scissorRect.left = 0;
	this->scissorRect.right = this->width;
	this->scissorRect.top = 0;
	this->scissorRect.bottom = this->height;
}
