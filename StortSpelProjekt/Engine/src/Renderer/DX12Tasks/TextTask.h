#ifndef TEXTTASK_H
#define TEXTTASK_H

#include "RenderTask.h"

class RootSignature;

namespace component
{
	class TextComponent;
}

class TextTask : public RenderTask
{
public:
	TextTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~TextTask();

	void SetTextComponents(std::vector<component::TextComponent*>* textComponents);

	void Execute();

private:
	std::vector<component::TextComponent*> m_TextComponents;

	void draw(ID3D12GraphicsCommandList5* commandList, component::TextComponent* tc);
};

#endif