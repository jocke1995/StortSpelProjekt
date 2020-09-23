#include "stdafx.h"
#include "RenderView.h"

RenderView::RenderView(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;

	createViewport();
	createScissorRect();
}

RenderView::~RenderView()
{
}

const D3D12_VIEWPORT* RenderView::GetViewPort() const
{
	return &m_Viewport;
}

const D3D12_RECT* RenderView::GetScissorRect() const
{
	return &m_ScissorRect;
}

void RenderView::createViewport()
{
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width = m_Width;
	m_Viewport.Height = m_Height;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
}

void RenderView::createScissorRect()
{
	m_ScissorRect.left = 0;
	m_ScissorRect.right = m_Width;
	m_ScissorRect.top = 0;
	m_ScissorRect.bottom = m_Height;
}
