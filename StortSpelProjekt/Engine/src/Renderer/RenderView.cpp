#include "stdafx.h"
#include "RenderView.h"

RenderView::RenderView(unsigned int width, unsigned int height)
{
	this->m_Width = width;
	this->m_Height = height;

	this->createViewport();
	this->createScissorRect();
}

RenderView::~RenderView()
{
}

const D3D12_VIEWPORT* RenderView::GetViewPort() const
{
	return &this->m_Viewport;
}

const D3D12_RECT* RenderView::GetScissorRect() const
{
	return &this->m_ScissorRect;
}

void RenderView::createViewport()
{
	this->m_Viewport.TopLeftX = 0.0f;
	this->m_Viewport.TopLeftY = 0.0f;
	this->m_Viewport.Width = this->m_Width;
	this->m_Viewport.Height = this->m_Height;
	this->m_Viewport.MinDepth = 0.0f;
	this->m_Viewport.MaxDepth = 1.0f;
}

void RenderView::createScissorRect()
{
	this->m_ScissorRect.left = 0;
	this->m_ScissorRect.right = this->m_Width;
	this->m_ScissorRect.top = 0;
	this->m_ScissorRect.bottom = this->m_Height;
}
