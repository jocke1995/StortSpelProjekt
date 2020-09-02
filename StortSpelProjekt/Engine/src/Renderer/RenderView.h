#ifndef RENDERVIEW_H
#define RENDERVIEW_H

class RenderView
{
public:
	RenderView(unsigned int width, unsigned int height);
	virtual ~RenderView();

	const D3D12_VIEWPORT* GetViewPort() const;
	const D3D12_RECT* GetScissorRect() const;

private:
	unsigned int m_Width = 0;
	unsigned int m_Height = 0;
	D3D12_VIEWPORT m_Viewport = {};
	D3D12_RECT m_ScissorRect = {};

	void createViewport();
	void createScissorRect();
};

#endif