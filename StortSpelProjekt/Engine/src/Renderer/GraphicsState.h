#ifndef GRAPHICSSTATE_H
#define GRAPHICSSTATE_H

#include "PipelineState.h"
class Shader;

// DX12 Forward Declarations
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

class GraphicsState : public PipelineState
{
public:
	GraphicsState(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd,
		const std::wstring& psoName);

	virtual ~GraphicsState();

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetGpsd() const;
	Shader* GetShader(ShaderType type) const;
private:
	Shader* m_pVS = nullptr;
	Shader* m_pPS = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC* m_pGPSD = nullptr;
};

#endif 