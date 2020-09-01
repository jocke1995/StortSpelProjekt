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
		LPCWSTR VSName, LPCWSTR PSName,
		D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd,
		LPCTSTR psoName);

	virtual ~GraphicsState();

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetGpsd() const;
	Shader* GetShader(ShaderType type) const;
private:
	Shader* VS = nullptr;
	Shader* PS = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd = nullptr;
};

#endif 