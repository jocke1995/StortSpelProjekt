#include "stdafx.h"
#include "Bloom.h"

#include "RenderTarget.h"
#include "../Misc/Window.h"


Bloom::Bloom(ID3D12Device5* device, DescriptorHeap* dhRTV, const HWND* hwnd)
{
	createBrightRenderTarget(device, dhRTV, hwnd);
}

Bloom::~Bloom()
{
	delete m_pRenderTarget;
}

const RenderTarget* const Bloom::GetRenderTarget() const
{
	return m_pRenderTarget;
}

void Bloom::createBrightRenderTarget(ID3D12Device5* device5, DescriptorHeap* dhRTV, const HWND* hwnd)
{
	RECT rect;
	unsigned int width = 0;
	unsigned int height = 0;
	if (GetWindowRect(*hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	m_pRenderTarget = new RenderTarget(device5, width, height, dhRTV);
}
