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
	unsigned int width = 0;
	unsigned int height = 0;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorRect = {};
	void CreateViewport();
	void CreateScissorRect();
};

#endif