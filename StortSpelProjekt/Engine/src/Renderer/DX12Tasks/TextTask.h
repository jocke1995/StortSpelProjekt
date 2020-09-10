#ifndef TEXTTASK_H
#define TEXTTASK_H

#include "RenderTask.h"

class RootSignature;

class TextTask : public RenderTask
{
public:
	TextTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		LPCWSTR VSName, LPCWSTR PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		LPCTSTR psoName);
	~TextTask();

	void Execute();

	void SubmitText(Text* text);

private:
	std::vector<Text*> m_TextVec;
};

#endif